/*
  Copyright 2014 Statoil ASA.

  This file is part of the Open Porous Media project (OPM).

  OPM is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  OPM is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with OPM.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdexcept>
#include <iostream>
#include <boost/filesystem.hpp>

#define BOOST_TEST_MODULE EclipseGridTests
#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>


#include <opm/parser/eclipse/Parser/Parser.hpp>

#include <opm/parser/eclipse/Deck/Section.hpp>
#include <opm/parser/eclipse/Deck/Deck.hpp>
#include <opm/parser/eclipse/Deck/DeckKeyword.hpp>

#include <opm/parser/eclipse/EclipseState/Grid/GridProperties.hpp>


BOOST_AUTO_TEST_CASE(Empty) {
    typedef Opm::GridProperties<int>::SupportedKeywordInfo SupportedKeywordInfo;
    std::vector<SupportedKeywordInfo> supportedKeywords =
        { SupportedKeywordInfo("SATNUM" , 0, "1"),
          SupportedKeywordInfo("FIPNUM" , 2, "1") };
    Opm::GridProperties<int> gridProperties( 10, 10, 100 , supportedKeywords);
    
    BOOST_CHECK( gridProperties.supportsKeyword("SATNUM") );
    BOOST_CHECK( gridProperties.supportsKeyword("FIPNUM") );
    BOOST_CHECK( !gridProperties.supportsKeyword("FLUXNUM") );
    BOOST_CHECK( !gridProperties.hasKeyword("SATNUM"));
    BOOST_CHECK( !gridProperties.hasKeyword("FLUXNUM"));
}



BOOST_AUTO_TEST_CASE(addKeyword) {
    typedef Opm::GridProperties<int>::SupportedKeywordInfo SupportedKeywordInfo;
    std::vector<SupportedKeywordInfo> supportedKeywords =
        { SupportedKeywordInfo("SATNUM" , 0, "1") };
    Opm::GridProperties<int> gridProperties( 100, 10 , 10 , supportedKeywords);
    
    BOOST_CHECK_THROW( gridProperties.addKeyword("NOT-SUPPORTED") , std::invalid_argument);

    BOOST_CHECK(  gridProperties.addKeyword("SATNUM"));
    BOOST_CHECK( !gridProperties.addKeyword("SATNUM"));

    BOOST_CHECK(  gridProperties.hasKeyword("SATNUM"));
}



BOOST_AUTO_TEST_CASE(getKeyword) {
    typedef Opm::GridProperties<int>::SupportedKeywordInfo SupportedKeywordInfo;
    std::vector<SupportedKeywordInfo> supportedKeywords =
        { SupportedKeywordInfo("SATNUM" , 0, "1") };
    Opm::GridProperties<int> gridProperties( 100,25,4 , supportedKeywords);
    std::shared_ptr<Opm::GridProperty<int> > satnum1 = gridProperties.getKeyword("SATNUM");
    std::shared_ptr<Opm::GridProperty<int> > satnum2 = gridProperties.getKeyword("SATNUM");
    
    BOOST_CHECK_EQUAL( satnum1.get() , satnum2.get());
    BOOST_CHECK_THROW( gridProperties.getKeyword("NOT-SUPPORTED") , std::invalid_argument );
}


