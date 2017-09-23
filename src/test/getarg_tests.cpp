#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-OLLIE");
    BOOST_CHECK(GetBoolArg("-OLLIE"));
    BOOST_CHECK(GetBoolArg("-OLLIE", false));
    BOOST_CHECK(GetBoolArg("-OLLIE", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-OLLIEo"));
    BOOST_CHECK(!GetBoolArg("-OLLIEo", false));
    BOOST_CHECK(GetBoolArg("-OLLIEo", true));

    ResetArgs("-OLLIE=0");
    BOOST_CHECK(!GetBoolArg("-OLLIE"));
    BOOST_CHECK(!GetBoolArg("-OLLIE", false));
    BOOST_CHECK(!GetBoolArg("-OLLIE", true));

    ResetArgs("-OLLIE=1");
    BOOST_CHECK(GetBoolArg("-OLLIE"));
    BOOST_CHECK(GetBoolArg("-OLLIE", false));
    BOOST_CHECK(GetBoolArg("-OLLIE", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noOLLIE");
    BOOST_CHECK(!GetBoolArg("-OLLIE"));
    BOOST_CHECK(!GetBoolArg("-OLLIE", false));
    BOOST_CHECK(!GetBoolArg("-OLLIE", true));

    ResetArgs("-noOLLIE=1");
    BOOST_CHECK(!GetBoolArg("-OLLIE"));
    BOOST_CHECK(!GetBoolArg("-OLLIE", false));
    BOOST_CHECK(!GetBoolArg("-OLLIE", true));

    ResetArgs("-OLLIE -noOLLIE");  // -OLLIE should win
    BOOST_CHECK(GetBoolArg("-OLLIE"));
    BOOST_CHECK(GetBoolArg("-OLLIE", false));
    BOOST_CHECK(GetBoolArg("-OLLIE", true));

    ResetArgs("-OLLIE=1 -noOLLIE=1");  // -OLLIE should win
    BOOST_CHECK(GetBoolArg("-OLLIE"));
    BOOST_CHECK(GetBoolArg("-OLLIE", false));
    BOOST_CHECK(GetBoolArg("-OLLIE", true));

    ResetArgs("-OLLIE=0 -noOLLIE=0");  // -OLLIE should win
    BOOST_CHECK(!GetBoolArg("-OLLIE"));
    BOOST_CHECK(!GetBoolArg("-OLLIE", false));
    BOOST_CHECK(!GetBoolArg("-OLLIE", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--OLLIE=1");
    BOOST_CHECK(GetBoolArg("-OLLIE"));
    BOOST_CHECK(GetBoolArg("-OLLIE", false));
    BOOST_CHECK(GetBoolArg("-OLLIE", true));

    ResetArgs("--noOLLIE=1");
    BOOST_CHECK(!GetBoolArg("-OLLIE"));
    BOOST_CHECK(!GetBoolArg("-OLLIE", false));
    BOOST_CHECK(!GetBoolArg("-OLLIE", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-OLLIE", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-OLLIE", "eleven"), "eleven");

    ResetArgs("-OLLIE -bar");
    BOOST_CHECK_EQUAL(GetArg("-OLLIE", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-OLLIE", "eleven"), "");

    ResetArgs("-OLLIE=");
    BOOST_CHECK_EQUAL(GetArg("-OLLIE", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-OLLIE", "eleven"), "");

    ResetArgs("-OLLIE=11");
    BOOST_CHECK_EQUAL(GetArg("-OLLIE", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-OLLIE", "eleven"), "11");

    ResetArgs("-OLLIE=eleven");
    BOOST_CHECK_EQUAL(GetArg("-OLLIE", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-OLLIE", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-OLLIE", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-OLLIE", 0), 0);

    ResetArgs("-OLLIE -bar");
    BOOST_CHECK_EQUAL(GetArg("-OLLIE", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-OLLIE=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-OLLIE", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-OLLIE=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-OLLIE", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--OLLIE");
    BOOST_CHECK_EQUAL(GetBoolArg("-OLLIE"), true);

    ResetArgs("--OLLIE=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-OLLIE", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noOLLIE");
    BOOST_CHECK(!GetBoolArg("-OLLIE"));
    BOOST_CHECK(!GetBoolArg("-OLLIE", true));
    BOOST_CHECK(!GetBoolArg("-OLLIE", false));

    ResetArgs("-noOLLIE=1");
    BOOST_CHECK(!GetBoolArg("-OLLIE"));
    BOOST_CHECK(!GetBoolArg("-OLLIE", true));
    BOOST_CHECK(!GetBoolArg("-OLLIE", false));

    ResetArgs("-noOLLIE=0");
    BOOST_CHECK(GetBoolArg("-OLLIE"));
    BOOST_CHECK(GetBoolArg("-OLLIE", true));
    BOOST_CHECK(GetBoolArg("-OLLIE", false));

    ResetArgs("-OLLIE --noOLLIE");
    BOOST_CHECK(GetBoolArg("-OLLIE"));

    ResetArgs("-noOLLIE -OLLIE"); // OLLIE always wins:
    BOOST_CHECK(GetBoolArg("-OLLIE"));
}

BOOST_AUTO_TEST_SUITE_END()
