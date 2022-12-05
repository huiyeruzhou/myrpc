cd /home/huiyeruzhou/Documents/WorkSpace/embeeded-rpc/myrpc/example
for i in {10..1}
do
./client $i $i > $i.txt
done
# 只要正在处理一个请求的时候来了新的请求,后面请求就都收不到返回.
# transport.m_fd = accept()
