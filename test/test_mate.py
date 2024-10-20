import csv
import os
import unittest
import pypixie


class TestMate(unittest.TestCase):

    def test_mate_in_1(self):
        epdfilepath = os.path.join("data", "mate_in_1.epd")
        with open(epdfilepath, newline='') as epdfile:
            epddata = csv.reader(epdfile, delimiter=';')
            for row in epddata:
                fen = row[0].split('bm')[0].strip() + " 0 1"
                print(f"Mate in 1: FEN = {fen}")
                with self.subTest(msg=f"Mate in 1: FEN = {fen}"):
                    eval = pypixie.search(fen, 1)
                    self.assertEqual(eval, (49000 - 1))

    def test_mate_in_2(self):
        epdfilepath = os.path.join("data", "mate_in_2.epd")
        with open(epdfilepath, newline='') as epdfile:
            epddata = csv.reader(epdfile, delimiter=';')
            for row in epddata:
                fen = row[0].split('bm')[0].strip() + " 0 1"
                print(f"Mate in 2: FEN = {fen}")
                with self.subTest(msg=f"Mate in 2: FEN = {fen}"):
                    eval = pypixie.search(fen, 3)
                    self.assertEqual(eval, (49000 - 3))

    def test_mate_in_3(self):
        epdfilepath = os.path.join("data", "mate_in_3.epd")
        with open(epdfilepath, newline='') as epdfile:
            epddata = csv.reader(epdfile, delimiter=';')
            for row in epddata:
                fen = row[0].split('bm')[0].strip() + " 0 1"
                print(f"Mate in 3: FEN = {fen}")
                with self.subTest(msg=f"Mate in 3: FEN = {fen}"):
                    eval = pypixie.search(fen, 5)
                    self.assertEqual(eval, (49000 - 5))

    def test_mate_in_4(self):
        epdfilepath = os.path.join("data", "mate_in_4.epd")
        with open(epdfilepath, newline='') as epdfile:
            epddata = csv.reader(epdfile, delimiter=';')
            for row in epddata:
                fen = row[0].split('bm')[0].strip() + " 0 1"
                print(f"Mate in 4: FEN = {fen}")
                with self.subTest(msg=f"Mate in 4: FEN = {fen}"):
                    eval = pypixie.search(fen, 7)
                    self.assertEqual(eval, (49000 - 7))

if __name__ == '__main__':
    unittest.main()