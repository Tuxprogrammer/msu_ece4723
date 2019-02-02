#!/usr/bin/python

import unittest
from weather import temperature

SIGNIFICANT_PLACES = 4


class KnownValues(unittest.TestCase):
    knownValues = ( (-273.15, -459.67),	# absolute zero
                    (-89, -128.2),	    # coldest recorded WX temp on earth's surface
                    (-17.777777777, 0),	# temp of Daniel Gabriel Fahrenheit's ice/salt mixture
                    (0, 32),		    # H20 freezes
                    (15, 59),		    # average earth WX temp
                    (36.8, 98.24),	    # average body temperature
                    (58, 136.4),	    # warmest recorded WX temp on earth's surface
                    (100, 212),		    # H20 boils
                    (5535, 9995) )	    # average WX temp on the sun's surface

    def testToFahrenheitKnownValues(self):
        """converting to Fahrenheit should give known result with known input"""
        for tempC, tempF in self.knownValues:
            result = temperature(celsius=tempC).fahrenheit()
            self.assertAlmostEqual(tempF, result, SIGNIFICANT_PLACES)

    def testToCelsiusKnownValues(self):
        """converting from Fahrenheit should give known back our known inputs"""
        for tempC, tempF in self.knownValues:
            result = temperature(fahrenheit=tempF).celsius()
            self.assertAlmostEqual(tempC, result, SIGNIFICANT_PLACES)


class ToFahrenheitBadInput(unittest.TestCase):
    def testTooSmall(self):
        """to Fahrenheit should fail with excessively negative Celsius input
		(below absolute zero)"""
        self.assertRaises(temperature.OutOfRangeError, temperature, celsius=-273.151)


class ToCelsiusBadInput(unittest.TestCase):
    def testTooSmall(self):
        """to Celsius should fail with excessively negative Fahrenheit input
		(below absolute zero)"""
        self.assertRaises(temperature.OutOfRangeError, temperature, fahrenheit=-459.671)


class SanityCheck(unittest.TestCase):
    def testSanityOne(self):
        """Celsius(Fahrenheit(n))==n for all n"""
        for tempC in range(-273, 6000):
            tempF = temperature(celsius=tempC).fahrenheit()
            result = temperature(fahrenheit=tempF).celsius()
            self.assertAlmostEqual(tempC, result, SIGNIFICANT_PLACES)

    def testSanityTwo(self):
        """Fahrenheit(Celsius(n))==n for all n"""
        for tempF in range(-459, 10000):
            tempC = temperature(fahrenheit=tempF).celsius()
            result = temperature(celsius=tempC).fahrenheit()
            self.assertAlmostEqual(tempF, result, SIGNIFICANT_PLACES)


if __name__ == "__main__":
    unittest.main()
