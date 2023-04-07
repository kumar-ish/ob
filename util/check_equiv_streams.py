import filecmp

def main():
    assert(filecmp.cmp("input1.stream", "input1.test.stream"))
    assert(filecmp.cmp("input2.stream", "input2.test.stream"))

if __name__ == "__main__":
    main()
