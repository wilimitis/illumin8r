float map(float input, float inputStart, float inputEnd, float outputStart, float outputEnd)
{
  float inputRange = inputEnd - inputStart;
  float outputRange = outputEnd - outputStart;
  return (input - inputStart) * outputRange / inputRange + outputStart;
}
