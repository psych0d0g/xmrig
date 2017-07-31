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


#include "net/Client.h"
#include "net/Network.h"
#include "net/strategies/SinglePoolStrategy.h"
#include "net/Url.h"
#include "Options.h"
#include "workers/Workers.h"


Network::Network(const Options *options) :
    m_donateActive(false),
    m_options(options),
    m_donate(nullptr),
    m_accepted(0),
    m_rejected(0)
{

    const std::vector<Url*> &pools = options->pools();

    m_strategy = new SinglePoolStrategy(pools.front(), this);

}



void Network::connect()
{
    m_strategy->connect();
}


void Network::stop()
{
}


void Network::onActive(Client *client)
{
}


void Network::onJob(Client *client, const Job &job)
{
    setJob(client, job);
}


void Network::onJobResult(const JobResult &result)
{
    m_strategy->submit(result);
}


void Network::onPause(IStrategy *strategy)
{
}


void Network::onResultAccepted(Client *client, int64_t seq, uint32_t diff, uint64_t ms, const char *error)
{
}


void Network::setJob(Client *client, const Job &job)
{
    Workers::setJob(job);
}

Network::~Network()
{
//    free(m_agent);
}
