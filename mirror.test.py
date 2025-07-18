import unittest
from mirror import *


class SquareDimension(unittest.TestCase):
    def test_empty(self):
        self.assertRaises(ValueError, lambda: square_dimension([]))

    def test_not_square(self):
        self.assertRaises(ValueError, lambda: square_dimension([1, 2, 3]))

    def test_square(self):
        actual = square_dimension([1,2,3,4])
        self.assertEqual(2, actual)


class MirrorHorizontally(unittest.TestCase):
    def test_square(self):
        actual = mirror_horizontally([1,2,3,4])
        self.assertEqual(actual, [2,1,4,3])


class MirrorVertically(unittest.TestCase):
    def test_square(self):
        actual = mirror_vertically([1,2,3,4])
        self.assertEqual(actual, [3,4,1,2])


class MirrorDiagonally(unittest.TestCase):
    def test_2x2(self):
        actual = mirror_diagonally([1,2,
                                    3,4])
        self.assertEqual(actual, [4,2,
                                  3,1])

    def test_3x3(self):
        actual = mirror_diagonally([1,2,3,
                                    4,5,6,
                                    7,8,9])
        self.assertEqual(actual, [9,6,3,
                                  8,5,2,
                                  7,4,1])


class Mirror(unittest.TestCase):
    def test_empty_direction(self):
        self.assertRaises(ValueError, lambda: mirror([1], None))
        self.assertRaises(ValueError, lambda: mirror([1], ''))

    def test_invalid_direction(self):
        self.assertRaises(ValueError, lambda: mirror([1], 'ZZZ'))

    def test_horizontal(self):
        actual = mirror([1,2,3,4], 'horizontally')
        self.assertEqual(actual, [2,1,4,3])

    def test_vertically(self):
        actual = mirror([1,2,3,4], 'vertically')
        self.assertEqual(actual, [3,4,1,2])

    def test_diagonally(self):
        actual = mirror([1,2,3,4], 'diagonally')
        self.assertEqual(actual, [4,2,3,1])


if __name__ == '__main__':
    unittest.main()
