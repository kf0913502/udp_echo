## Build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM=ninja -G Ninja -S <project_directory> -B <build_directory>

cmake --build <build_directory> -j 8

The binary will be in the <build_directory>. Note these are absolute paths.

## Usage

first start a server
      udp_echo -s <listen_port>

then start a client
      udp_echo -c <server_ip> <server_port> <n_packets>

