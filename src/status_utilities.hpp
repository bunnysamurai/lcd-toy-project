#ifndef STATUS_UTILITIES_HPP
#define STATUS_UTILITIES_HPP

class BlinkStatus final
{
public:
  struct Milliseconds
  {
    int value;
  };
  BlinkStatus(Milliseconds);

  void blink_forever();

private:
  Milliseconds m_period;
  int m_state{0x01};

  void init_gpio();
};

#endif