/*
 * DATA PULLED FROM ATMEL328P DATASHEET
 * 
 * We are using 16 bit timer 1 to generate a PWM signal.
 *
 * Key registers:
 * - TCNT1 - 16 bit timer counter, the prescaled timer clock increments this
 * - OCR1A - 16 bit buffered output compare register (the duty cycle is set with this), when matched with TCNT1 toggles pin 9 if enabled
 * - OCR1B - 16 bit buffered output compare register (the duty cycle is set with this), when matched with TCNT1 toggles pin 10 if enabled (not used, set to 0x0000U)
 * - TCCR1A - 8 bit configuration register, set bit COM1A1 to 1, COM1A0 to 1, WGM11 to 1 and WGM10 to 0
 * - TCCR1B - 8 bit configuration register, set bit WGM13 to 1, WGM12 to 1, CS12 to 0, CS11 to 0 and CS10 to 1
 * - TCCR1C - 8 bit configuration register, set bit 
 * - ICR1 - 16 bit input capture register, usually used for input capture but in this case will be used for setting the period of the timer
 * - TIMSK1 - 8 bit interrupt mask register, (interrupt not used, set to 0x00U)
 * - TIFR1 - 8 bit interrupt flag register (set to 0x00U)
 *
 * The buffered registers are usually updated at the beggining or the end of the timer period.
 * COM1A1 option 11 specifies that the duty cycle will work in the inverse mode which means that the cycle begins with the off state and is then switched on upon encountering OCR match.
 * WGM1 option 1110 specifies that the timer works in fast PWM mode and the timer period can be written in register ICR1.
 * CS1 option 001 specifies that the timer uses the internal clock with no prescaling.
 *
 * As we are using pin 9, we need to set it to output. Use the register:
 * - DDRB - 8 bit data direction register, set bit DDB1 bit to 1 (pin 9 pin mode output)
 *
 * Also, we are using the 10 bit successive aproximation ADC to convert analog inputs into data.
 *
 * Key registers:
 * - ADMUX - 8 bit multiplexer selection register, set bit REFS1 to 0, set bit REFS0 to 1, set bit MUX3 to 0, set bit MUX2 to 0, set bit MUX1 to 0 and set bit MUX0 to 0 or 1 depending if using channel 0 or 1
 * - ADCSRA - 8 bit status register, set bit ADEN to 1 (enable ADC), set bit ADSC (start conversion) when starting conversion, set bit ADIE to 0 and finally ADPS2, ADPS1 and ADSP0 to 1, 0 and 1
 *          - when conversion finishes ADSC bit will be set to 0
 * - DIDR0 - 8 bit digital input disable register, set bit ADC0D and ADC1D to 1 to disable digital input
 */

/* Timer 1 input clock */
#define TIMER_1_FREQ_HZ (16000000U)

/* Wanted PWM frequency */
#define PWM_FREQ_HZ (50000U)

/* Timer 1 ICR1 max register value - top value */
#define ICR1_TOP_REG_VAL ((TIMER_1_FREQ_HZ / PWM_FREQ_HZ) - 1)

/* Target output buck voltage (millivolts) */
#define V_OUT_BUCK (5000U)

/* Buck output max */
#define V_OUT_BUCK_MAX (12000U)

/* Resistor R1 (Ohms) */
#define R1_OHM (10000U)

/* Resistor R2 (Ohms) */
#define R2_OHM (1000U)

/* ADC max voltage (millivolts) */
#define V_ADC_MAX (5000U)

/* Pin D9 and timer 1 setup */
void setup_pwm();

/* Setup analog pins A0 and A1 */
void setup_analog_read();

/* Read converted value */
inline uint16_t analog_read(uint16_t channel);

const uint32_t r1_resistance = R1_OHM;
const uint32_t r2_resistance = R2_OHM;
const uint32_t adc_voltage_in_max = V_ADC_MAX;

void setup()
{
  Serial.begin(115200);
  setup_pwm();
  setup_analog_read();
}

void loop()
{
  uint16_t adc_0_result = 0;
  uint16_t adc_1_result = 0;
  uint16_t ocr1a_reg_value = OCR1A;

  uint32_t adc_target_value = 0;
  uint32_t r2_voltage = 0;
  uint32_t voltage_buck_out = V_OUT_BUCK;

  // Read both analog pins A0 and A1
  adc_0_result = analog_read(0);
  adc_1_result = analog_read(1);

  // Calculate target voltage using pin A1
  voltage_buck_out = V_OUT_BUCK_MAX * (uint32_t)adc_1_result / 1023;

  // Calculate target analog value that should be on pin A0 using the target voltage
  r2_voltage = voltage_buck_out * r2_resistance / (r1_resistance + r2_resistance);
  adc_target_value = 1023 * r2_voltage / V_ADC_MAX;

  // If too little voltage, increase duty cycle on MOSFET (decreasemax  on BJT)
  if (adc_0_result < adc_target_value && ocr1a_reg_value < ICR1_TOP_REG_VAL) ocr1a_reg_value++;
  // If too much voltage, decrease duty cycle on MOSFET (increase on BJT)
  else if (adc_0_result > adc_target_value && ocr1a_reg_value > 0) ocr1a_reg_value--;

  // Set new duty cycle
  OCR1A = ocr1a_reg_value;

  Serial.print("Output voltage (millivolts): ");
  Serial.println(voltage_buck_out);
}

void setup_pwm()
{
  DDRB = 0x00U | (1 << DDB1);         // Pin 9 as output
  
  OCR1A = 0x0000U;                    // Output compare register A value, will be updated lately
  OCR1B = 0x0000U;                    // Output compare register B value
  TCNT1 = 0x0000U;                    // Reset timer 1 counter
  TIMSK1 = 0x00U;                     // Disable all timer 1 interrupts
  TIFR1 = 0x00U;                       // Clear all timer 1 interrupt flags

  // Set timer 1 configuration, enable inverse fast PWM by setting COM1A1 to 10, WGM1 to 1110 and CS1 to 001
  TCCR1A = 0x00U | (1 << COM1A1) | (1 << COM1A0) | (1 << WGM11) | (0 << WGM10);
  TCCR1B = 0x00U | (1 << WGM13) | (1 << WGM12) | (0 << CS12) | (0 << CS11) | (1 << CS10);
  TCCR1C = 0x00U;

  // Set periodmax 
  ICR1 = ICR1_TOP_REG_VAL;

  // Set initial omax utput compare register A value
  OCR1A = 0;
}

void setup_analog_read()
{
  // Reset pin channel multiplexor
  ADMUX = 0x00U;

  // Enable ADC and set prescaler to 64 (from 16 MHz)
  ADCSRA = 0x00U | (1 << ADEN) | (1 << ADPS2) | (0 << ADPS1) | (1 << ADPS0);

  // Disable digital input on pins A0 and A1
  DIDR0 = 0x00U | (1 << ADC1D) | (1 << ADC0D);
}

inline uint16_t analog_read(uint16_t channel)
{
  uint16_t adc_result = 0;

  /* AVcc as reference and select MUX channel */
  ADMUX = 0x00U | (0 << REFS1) | (1 << REFS0) | (0xF & channel);

  /* Start conversion */
  ADCSRA |= (1 << ADSC);

  /* Wait for conversion */
  while (ADCSRA & (1 << ADSC));

  /* Read result */
  adc_result = ADC;

  return adc_result;
}
