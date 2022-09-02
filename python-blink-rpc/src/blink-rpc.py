import time
from msgpackrpc import Address as RpcAddress, Client as RpcClient, error as RpcError

publish_interval = 1

# The M4 Proxy address needs to be mapped via Docker's extra hosts
m4_proxy_address = 'm4-proxy'
m4_proxy_port = 5001


def get_data_from_m4():
    rpc_address = RpcAddress(m4_proxy_address, m4_proxy_port)

    data = ()

    try:
        rpc_client = RpcClient(rpc_address)
        count = rpc_client.call('count')

        rpc_client = RpcClient(rpc_address)
        led = rpc_client.call('led')

        data = count, led

    except RpcError.TimeoutError:
        print("Unable to retrieve data from the M4.")

    return data


if __name__ == '__main__':

    print()
    print("============================================")
    print("==       Portenta X8 M4 output            ==")
    print("============================================")
    print()

    try:
        while True:
            data = get_data_from_m4()
            if len(data) > 0:
                print("count: ", data[0])
                print("led: ", data[1])
            else:
                print("no data!")
            time.sleep(publish_interval)
    except KeyboardInterrupt:
        print('Stopped.')
