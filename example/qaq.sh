cd /home/huiyeruzhou/Documents/WorkSpace/embeeded-rpc/myrpc/example
./server &
sleep 1
./client 1 1 


# 只要正在处理一个请求的时候来了新的请求,后面请求就都收不到返回.
# transport.m_fd = accept()
