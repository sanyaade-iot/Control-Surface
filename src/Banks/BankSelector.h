#ifndef BANKSELECTOR_H_
#define BANKSELECTOR_H_

#include "./Bank.h"
#include "../Selectors/Selector.h"

/**
 * @brief A class for changing the setting of a Bank 
 * depending on the selection of a Selector.
 * 
 * Can be used for selecting fader banks, 
 * changing the channel of MIDI_Control_Element%s, 
 * transposing notes, displaying different tracks 
 * for MIDI_Input_Element%s, etc.
 * 
 * Refer to the documentation of Selector for information
 * on the different modes of operation.
 */
class BankSelector : public Selector
{
public:
  template <class... Args>
  BankSelector(Bank &bank, Args... args) : Selector(args...), bank(bank) {}
  // initializer_lists are not supported with variadic templates, so overload them manually
  template <class T, size_t N>
  BankSelector(Bank &bank, const T (&switchPins)[N]) : Selector((const pin_t (&)[N])switchPins), bank(bank) {} // Multiple buttons, no LEDs
  template <class T, class S, size_t N, size_t M>
  BankSelector(Bank &bank, const T (&switchPins)[N], const S (&ledPins)[M]) : Selector((const pin_t (&)[N])switchPins, (const pin_t (&)[M])ledPins), bank(bank) {} // One (+1), two (+1, -1) or multiple buttons, multiple LEDs
  template <class T, class S, size_t N>
  BankSelector(Bank &bank, const T (&switchPins)[N], S nb_settings) : Selector((const pin_t (&)[N])switchPins, (size_t)nb_settings), bank(bank) {} // One (+1) or two (+1, -1) buttons, no LEDs

private:
  Bank &bank;
  void refreshImpl(uint8_t newSetting)
  {
    bank.setBankSetting(newSetting);
  }
};

#endif // BANKSELECTOR_H_