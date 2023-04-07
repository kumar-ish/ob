from sys import stdin
from collections import defaultdict

# Takes in the debug log in the stdin that was partially implemented in the starter code / completed by me
# Can be retrieved from `depthify` program in the stderr stream using the DEBUG envvar
def main():
    order_change_map = defaultdict(lambda: defaultdict(list))

    for ix in stdin:
        scrappyJson = ix.split("/")
        print(scrappyJson)
        order = eval(scrappyJson[2])

        order_change_map[order['symbol']][order['order_id']].append((scrappyJson[1], order['side']))
    
    for symbol in order_change_map:
        for order_id in order_change_map[symbol]:
            # Verify (empirically) that you can't change order sides and that the order_id is unique for each side
            print(order_id, order_change_map[symbol][order_id])
        

if __name__ == "__main__":
    main()