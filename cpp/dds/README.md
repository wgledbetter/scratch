# DDS Playground

Just learning.
Should really have been making more notes today.
Not that I'd go back and read them, it just helps with retention.

## TODO

- [x] TCP Example.
- [ ] Implement late joiners as two executables. Maybe with pub-sub in both so the main publisher knows how long to hang out.
- [x] Combine TCP and late joiners.
- [ ] Make one side of the TCP example in python. <https://fast-dds.docs.eprosima.com/en/latest/fastdds/getting_started/simple_python_app/simple_python_app.html>
- [ ] TCPv6
- [ ] UDPv4 and UDPv6
- [ ] Dynamic XML example.
- [ ] Can OpenDDS talk to FastDDS?


## Notes

- For the TCP example, the whitelist IP is the IP on the local machine that is allowed to handle traffic.
- TCP multi-machine settings:
  - `./dds-pub --mode TCPv4 -p $DDS_PORT -w $MY_IP`
  - `./dds-sub --mode TCPv4 -p $DDS_PORT -w $MY_IP --ip $THEIR_IP`
- Late joiners works over UDP. There is some magic going on here.
