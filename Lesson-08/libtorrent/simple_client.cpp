// simple torrent client

#include <stdlib.h>
#include <boost/make_shared.hpp>
#include "libtorrent/entry.hpp"
#include "libtorrent/bencode.hpp"
#include "libtorrent/session.hpp"
#include "libtorrent/torrent_info.hpp"

int main(int argc, char* argv[]) try
{
	if (argc != 2)
	{
		std::cerr << "usage: ./simple_client torrent-file\n"
			"to stop the client, press return.\n";
		return 1;
	}

	lt::session s;
	lt::add_torrent_params p;
	p.save_path = "./";
	lt::error_code ec;
	p.ti = boost::make_shared<lt::torrent_info>(std::string(argv[1]), 0);
	s.add_torrent(p);

	// wait for the user to end
	char a;
	scanf("%c\n", &a);
	return 0;
}
catch (std::exception const& e)
{
	std::cerr << "ERROR: " << e.what() << "\n";
}

