//------------------------------------------------------------------------------
/*
  This file is part of trackabled: https://github.com/trackable/trackabled
  Copyright (c) 2012-2015 Trackable Labs Inc.

  Permission to use, copy, modify, and/or distribute this software for any
  purpose  with  or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THE  SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH  REGARD  TO  THIS  SOFTWARE  INCLUDING  ALL  IMPLIED  WARRANTIES  OF
  MERCHANTABILITY  AND  FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY  SPECIAL ,  DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER  RESULTING  FROM  LOSS  OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION  OF  CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
//==============================================================================

#include <BeastConfig.h>
#include <test/jtx.h>
#include <trackable/beast/unit_test.h>
#include <trackable/protocol/Feature.h>

namespace trackable {
namespace test {

class DeliverMin_test : public beast::unit_test::suite
{
public:
    void
    test_convert_all_of_an_asset(std::initializer_list<uint256> fs)
    {
        testcase("Convert all of an asset using DeliverMin");

        using namespace jtx;
        auto const gw = Account("gateway");
        auto const USD = gw["USD"];

        {
            Env env(*this, with_features(fs));
            env.fund(XRP(10000), "alice", "bob", "carol", gw);
            env.trust(USD(100), "alice", "bob", "carol");
            env(pay("alice", "bob", USD(10)), delivermin(USD(10)),  ter(temBAD_AMOUNT));
            env(pay("alice", "bob", USD(10)), delivermin(USD(-5)),
                txflags(tfPartialPayment),                          ter(temBAD_AMOUNT));
            env(pay("alice", "bob", USD(10)), delivermin(XRP(5)),
                txflags(tfPartialPayment),                          ter(temBAD_AMOUNT));
            env(pay("alice", "bob", USD(10)),
                delivermin(Account("carol")["USD"](5)),
                txflags(tfPartialPayment),                          ter(temBAD_AMOUNT));
            env(pay("alice", "bob", USD(10)), delivermin(USD(15)),
                txflags(tfPartialPayment),                          ter(temBAD_AMOUNT));
            env(pay(gw, "carol", USD(50)));
            env(offer("carol", XRP(5), USD(5)));
            env(pay("alice", "bob", USD(10)), paths(XRP),
                delivermin(USD(7)), txflags(tfPartialPayment),
                sendmax(XRP(5)),                                   ter(tecPATH_PARTIAL));
            env.require(balance("alice", XRP(9999.99999)));
            env.require(balance("bob", XRP(10000)));
        }

        {
            Env env(*this, with_features(fs));
            env.fund(XRP(10000), "alice", "bob", gw);
            env.trust(USD(1000), "alice", "bob");
            env(pay(gw, "bob", USD(100)));
            env(offer("bob", XRP(100), USD(100)));
            env(pay("alice", "alice", USD(10000)), paths(XRP),
                delivermin(USD(100)), txflags(tfPartialPayment),
                sendmax(XRP(100)));
            env.require(balance("alice", USD(100)));
        }

        {
            Env env(*this, with_features(fs));
            env.fund(XRP(10000), "alice", "bob", "carol", gw);
            env.trust(USD(1000), "bob", "carol");
            env(pay(gw, "bob", USD(200)));
            env(offer("bob", XRP(100), USD(100)));
            env(offer("bob", XRP(1000), USD(100)));
            env(offer("bob", XRP(10000), USD(100)));
            env(pay("alice", "carol", USD(10000)), paths(XRP),
                delivermin(USD(200)), txflags(tfPartialPayment),
                sendmax(XRP(1000)),                                 ter(tecPATH_PARTIAL));
            env(pay("alice", "carol", USD(10000)), paths(XRP),
                delivermin(USD(200)), txflags(tfPartialPayment),
                sendmax(XRP(1100)));
            env.require(balance("bob", USD(0)));
            env.require(balance("carol", USD(200)));
        }

        {
            Env env(*this, with_features(fs));
            env.fund(XRP(10000), "alice", "bob", "carol", "dan", gw);
            env.trust(USD(1000), "bob", "carol", "dan");
            env(pay(gw, "bob", USD(100)));
            env(pay(gw, "dan", USD(100)));
            env(offer("bob", XRP(100), USD(100)));
            env(offer("bob", XRP(1000), USD(100)));
            env(offer("dan", XRP(100), USD(100)));
            env(pay("alice", "carol", USD(10000)), paths(XRP),
                delivermin(USD(200)), txflags(tfPartialPayment),
                sendmax(XRP(200)));
            env.require(balance("bob", USD(0)));
            env.require(balance("carol", USD(200)));
            env.require(balance("dan", USD(0)));
        }
    }

    void
    run()
    {
        test_convert_all_of_an_asset({});
        test_convert_all_of_an_asset({featureFlow});
        test_convert_all_of_an_asset({featureFlow, fix1373});
        test_convert_all_of_an_asset(
            {featureFlow, fix1373, featureFlowCross});
    }
};

BEAST_DEFINE_TESTSUITE(DeliverMin,app,trackable);

} // test
} // trackable
