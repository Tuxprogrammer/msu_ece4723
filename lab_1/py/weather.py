class temperature:

    absZeroC = -273.15
    absZeroF = -459.67

    class Error(Exception):
        pass

    class OutOfRangeError(Error):
        pass

    def __init__(self, celsius=0, fahrenheit=0):
        if celsius < self.absZeroC or fahrenheit < self.absZeroF:
            raise self.OutOfRangeError("temperature out of range.")
        else:
            self.tempC = celsius
            self.tempF = fahrenheit

    def fahrenheit(self):
        self.tempF = ((9.0 / 5.0) * self.tempC) + 32
        return self.tempF

    def celsius(self):
        self.tempC = (self.tempF - 32) * (5.0 / 9.0)
        return self.tempC
