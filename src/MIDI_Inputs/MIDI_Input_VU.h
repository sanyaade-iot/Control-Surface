#ifndef MIDI_Input_VU_H_
#define MIDI_Input_VU_H_

#include "MIDI_Input_Element.h"
#include "../ExtendedInputOutput/ExtendedInputOutput.h"
#include "../Helpers/StreamOut.h"
#include "../Helpers/Copy.hpp"
#include <string.h>

using namespace ExtIO;

class MCU_VU : public MIDI_Input_Element_ChannelPressure
{
  public:
    MCU_VU(uint8_t track, uint8_t nb_tracks = 1, bool decay = true, unsigned int decayTime = 300)
        : MIDI_Input_Element_ChannelPressure(track - 1, 1, nb_tracks, 1),
          decay(decay), decayTime(decayTime)
    {
        initBuffer();
    }

    ~MCU_VU()
    {
        free(values);
    }
    void reset()
    {
        for (uint8_t i = 0; i < this->nb_addresses; i++)
        {
            setValue(i, 0);
            clearOverload(i);
        }
        display();
    }

    bool updateImpl(const MIDI_message_matcher &midimsg)
    {
        uint8_t targetID = midimsg.data1 >> 4;

        if (!matchID(targetID))
            return false;

        uint8_t index = (targetID - this->address) / tracksPerBank;

        uint8_t data = midimsg.data1 & 0xF;
        if (data == 0xF) // clear overload
            clearOverload(index);
        else if (data == 0xE) // set overload
            setOverload(index);
        else if (data == 0xD) // not implemented
            ;
        else // new peak value
            setValue(index, data);

#ifdef DEBUG
        DEBUG_OUT << "address = " << this->address << endl;
        DEBUG_OUT << "index = " << index << endl;
        DEBUG_OUT << "targetID = " << targetID << endl;
        DEBUG_OUT << "VU value: " << getValue(index) << endl;
        DEBUG_OUT << "addressOffset = " << addressOffset << endl;
#endif
        // DEBUG_OUT << "<< " << hex << (midimsg.channel | midimsg.type) << ' ' << midimsg.data1 << dec << tab << millis() << endl;

        display();
        return true;
    }

    void refresh()
    {
        if (decay && ((millis() - prevDecayTime) > decayTime))
        {
            for (uint8_t i = 0; i < nb_addresses; i++)
                if (getValue(i) > 0)
                    setValue(i, getValue(i) - 1);
            prevDecayTime = millis();
            display();
        }
    }

    uint8_t getValue()
    {
        return getValue(addressOffset);
    }
    uint8_t getOverload()
    {
        return getOverload(addressOffset);
    }

  protected:
    uint8_t *values = nullptr;
    const bool decay;
    const unsigned long decayTime;
    unsigned long prevDecayTime = 0;

    void initBuffer()
    {
        if (values != nullptr)
            return;
        values = (uint8_t *)malloc(sizeof(uint8_t) * nb_addresses);
        memset(values, 0, sizeof(uint8_t) * nb_addresses);
    }

    void setValue(uint8_t address, uint8_t value)
    {
        values[address] &= 0xF0;
        values[address] |= value;
        prevDecayTime = millis();
    }
    uint8_t getValue(uint8_t address)
    {
        return values[address] & 0x0F;
    }
    void setOverload(uint8_t address)
    {
        values[address] |= 0xF0;
    }
    void clearOverload(uint8_t address)
    {
        values[address] &= 0x0F;
    }
    bool getOverload(uint8_t address)
    {
        return values[address] & 0xF0;
    }
    bool matchID(uint8_t targetID)
    {
        int8_t addressDiff = targetID - this->address;
#ifdef DEBUG
        DEBUG_OUT << "VU meter target ID: " << targetID << endl
              << (((addressDiff >= 0) && (addressDiff < nb_addresses * tracksPerBank) && (addressDiff % tracksPerBank == 0)) ? "match" : "no match") << endl;
#endif
        return (addressDiff >= 0) && (addressDiff < nb_addresses * tracksPerBank) && (addressDiff % tracksPerBank == 0);
    }
};

//----------------------------------------------------------------------------------------------------------------------------------------//

class MCU_VU_LED : public MCU_VU
{
  public:
    template <size_t N>
    MCU_VU_LED(const pin_t (&LEDs)[N], uint8_t address, uint8_t nb_addresses, bool decay = true)
        : MCU_VU(address, nb_addresses, decay), overloadpin(0), length(N), overload(false)
    {
        static_assert(N <= 12, "Error: the maximum number of LEDs in the VU meter is 12. ");
        this->LEDs = new pin_t[length];
        copy(this->LEDs, LEDs);
        for (pin_t pin = 0; pin < length; pin++)
            ExtIO::pinMode(LEDs[pin], OUTPUT);
    }
    /*
    MCU_VU_LED(pin_t start, uint8_t length, pin_t overloadpin, uint8_t address, uint8_t nb_addresses, bool decay = true)
        : MCU_VU(address, nb_addresses, decay), start(start), length(length < 12 ? length : 12),
          overloadpin(overloadpin), overload(true)
    {
        for (pin_t pin = 0; pin < length; pin++)
        {
            pinMode(start + pin, OUTPUT);
        }
        pinMode(overloadpin, OUTPUT);
    }
    */
    ~MCU_VU_LED()
    {
        delete[] LEDs;
    }

  protected:
    const uint8_t length;
    const pin_t overloadpin;
    pin_t *LEDs;
    const bool overload;

    const static uint8_t floorCorrection = 5;

    void display()
    {
        for (uint8_t pin = 0; pin < (getValue(addressOffset) * length + floorCorrection) / 12; pin++)
            digitalWrite(LEDs[pin], HIGH);
        for (uint8_t pin = (getValue(addressOffset) * length + floorCorrection) / 12; pin < length; pin++)
            digitalWrite(LEDs[pin], LOW);
        if (overload)
            digitalWrite(overloadpin, getOverload(addressOffset));
    }
};

#endif // MIDI_Input_VU_H_