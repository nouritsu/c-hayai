def main():
    multi_page_to_long("./test/multi_page_big.txt",
                       "./test/long_horizontal_big.txt")


def multi_page_to_long(inp, out):
    with open(inp, 'r') as r:
        text = r.read()
        text = text.replace("\r\n", " ").replace("\n", " ")
        with open(out, 'w') as w:
            w.write(text)
            w.write('\n')


if __name__ == "__main__":
    main()
