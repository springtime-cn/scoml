import socket


def get_max_num(num):
    """
    描述: 根据十进制位数计算其表示最大数值
    :param num: 输入十进制位数
    :return: 最大数值
    """
    if not isinstance(num, int):
        raise RuntimeError("num should be int.")
    if num <= 0:
        raise RuntimeError("num should exceed 0.")
    str_num = ''
    for i in range(num):
        str_num += '9'
    return int(str_num)


def scoml_var_send(handle_socket, msg, ori_len=12):
    """
    描述: 可变长度 socket 接收函数
    :param handle_socket: 指用哪个 socket 来发送数据
    :param msg: bytes, or bytes from str with encoding ascii
    :param ori_len: default 12
    :return: 0
    """
    # if not (isinstance(msg, str) or isinstance(msg, bytes)):
    #     raise RuntimeError("msg data type must be str or bytes")
    # elif isinstance(msg, str):
    #     msg = bytes(msg, 'utf-8')
    if not isinstance(msg, bytes):
        raise RuntimeError("msg data type must be bytes")
    if len(msg) > get_max_num(ori_len):
        raise RuntimeError("msg data exceeds the length which ori_len can describes. "
                           "You should increase the ori_len parameter in both server and client.")
    ori_sent = 0
    ori_msg = bytes(str(len(msg)).zfill(ori_len), 'ascii')
    while ori_sent < ori_len:
        sent = handle_socket.send(ori_msg[ori_sent:])
        if sent == 0:
            raise RuntimeError("socket connection broken")
        ori_sent += sent
    msg_sent = 0
    while msg_sent < len(msg):
        sent = handle_socket.send(msg[msg_sent:])
        if sent == 0:
            raise RuntimeError("socket connection broken")
        msg_sent += sent
    return 0


def scoml_var_receive(handle_socket, ori_len=12):
    """
    描述: 可变长度 socket 发送函数 
    :param handle_socket: 指用哪个 socket 来接收数据
    :param ori_len: 表示数据体长度的数位, 默认值 12
    :return: bytes
    """
    ori_chunks = []
    ori_recd = 0
    while ori_recd < ori_len:
        recd = handle_socket.recv(min(ori_len-ori_recd, 2048))
        if recd == b'':
            raise RuntimeError("socket connection broken")
        ori_chunks.append(recd)
        ori_recd += len(recd)
    msg_len = int(b''.join(ori_chunks).decode('ascii'))

    chunks = []
    msg_recd = 0
    while msg_recd < msg_len:
        recd = handle_socket.recv(min(msg_len - msg_recd, 2048))
        if recd == b'':
            raise RuntimeError("socket connection broken")
        chunks.append(recd)
        msg_recd += len(recd)
        if msg_recd > get_max_num(ori_len):
            raise RuntimeError("received data exceeds the length which ori_len can describes. "
                               "You should increase the ori_len parameter in both server and client.")
    return b''.join(chunks)


class ScomlServerSocket:
    """
    描述: 服务端 socket 类
    """
    def __init__(self):
        pass

    def response(self, msg):
        """
        execute function
        :param msg: received msg of bytes type
        :return: response msg of bytes type
        """
        raise NotImplementedError("You should write your execute code here.")

    def start(self, port, ori_len=12):
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.bind(("localhost", port))
        sock.listen()
        print("server running at port "+str(port))
        while True:
            try:
                handle_socket, addr = sock.accept()
                message = scoml_var_receive(handle_socket, ori_len)
                answer = self.response(message)
                if answer is None:
                    continue
                if not isinstance(answer, bytes):
                    raise RuntimeError("answer is not bytes.")
                scoml_var_send(handle_socket, answer, ori_len)
                handle_socket.close()
            except Exception:
                print("error when response.")
                sock.close()
                return


def scoml_var_request(ip, port, msg, ori_len=12):
    """

    :param ip: str
    :param port:
    :param msg: bytes, or bytes from str with encoding utf-8
    :param ori_len:
    :return:
    """
    if not isinstance(msg, bytes):
        return "msg data type must be bytes"
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((ip, port))
    scoml_var_send(sock, msg)
    answer = scoml_var_receive(sock, ori_len)
    sock.close()
    return answer

