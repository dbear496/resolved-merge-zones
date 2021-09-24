### Installing
To my knowledge, the only dependencies are git and whatever is required to build kernel modules. I believe these can be installed with `sudo apt install build-essential linux-headers-$(uname -r) make git`

Run as normal user:
```
git clone https://github.com/dbear496/resolved-merge-zones.git
cd resolved-merge-zones
make
```
Then run with root or sudo:
```
make install
depmod -a
modprobe resolved-merge-zones
```
Note: `depmod -a` only need be run if the `make install` command reports that depmod was skipped.

### How It Works
systemd-resolved assumes that all DNS servers are equal. Therefore, when a DNS server returns an NXDOMAIN rcode (no such domain), resolved takes that as authoritative and does not attempt other servers. However, this can cause problems if some servers _are_ different; the notable case of this is when using a private DNS server that is specific to a LAN in addition to a public DNS server for backup. In this case, resolved may get stuck on using the backup server and not properly resolve LAN-specific names.

This kernel module inserts a listener on the network stack that detects incoming NXDOMAIN error codes. Whenever an NXDOMAIN code is detected (rcode 3), it changes it to a different error code (I chose rcode 4). When resolved receives this other error code, it assumes that something is wrong with the server and switches servers. This effectively changes the behavior of resolved to try all servers before concluding that a domain does not exist.
