import csv
import os
import unittest
import pypixie

class TestPerft(unittest.TestCase):

    def test_perft(self):
        perftfilepath = os.path.join("data", "standard.epd")
        with open(perftfilepath, newline='') as perftfile:
            perftdata = csv.reader(perftfile, delimiter=';')
            failed_perft = {}
            for row in perftdata:
                fen = row[0].strip()
                for depths in range(1, len(row)):
                    depth, expected_nodes = row[depths].strip().split(' ')
                    print(f"\nPerft: FEN = {fen}; Depth = {depth}")
                    with self.subTest(msg=f"Perft: FEN = {fen}; Depth = {depth}"):
                        if(failed_perft.get(fen, None) is None):
                            result = pypixie.perft(fen, int(depth[1]))
                            try:
                                self.assertEqual(result, int(expected_nodes))
                            except AssertionError:
                                failed_perft[fen] = int(depth[1])
                                self.fail(f"Failed Perft: FEN = {fen}; Depth = {depth}")
                        else:
                            self.skipTest(f"Skipped Perft: FEN = {fen}; Depth = {depth}")

if __name__ == '__main__':
    unittest.main()