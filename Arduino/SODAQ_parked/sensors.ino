float getTemperature()
{
  float mVolts = (float)analogRead(TEMP_SENSOR) * 3300.0 / 1023.0;
  return (mVolts - 500.0) / 10.0;
}

