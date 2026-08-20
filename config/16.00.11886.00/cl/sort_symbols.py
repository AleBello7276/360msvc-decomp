import csv
import sys

def sort_symbols(path):
    with open(path, "r", newline="", encoding="utf-8") as f:
        rows = list(csv.reader(f))

    header = rows[0]
    data = [r for r in rows[1:] if r and len(r) >= 4]

    def key(r):
        try:
            return int(r[0], 16)
        except ValueError:
            return -1

    data.sort(key=key)

    with open(path, "w", newline="", encoding="utf-8") as f:
        w = csv.writer(f)
        w.writerow(header)
        w.writerows(data)

if __name__ == "__main__":
    path = sys.argv[1] if len(sys.argv) > 1 else r"symbols.csv"
    sort_symbols(path)
    print(f"Sorted {path}")
