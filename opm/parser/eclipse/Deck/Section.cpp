/*
  Copyright 2013 Statoil ASA.

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

#include <iostream>
#include <exception>
#include <algorithm>
#include <cassert>
#include <set>
#include <string>

#include <opm/parser/eclipse/Deck/Deck.hpp>
#include <opm/parser/eclipse/Deck/Section.hpp>

namespace Opm {
    Section::NullStream Section::nullStream;

    Section::Section(DeckConstPtr deck, const std::string& startKeywordName)
        : m_name(startKeywordName)
    {
        populateSection(deck, startKeywordName);
    }

    void Section::populateSection(DeckConstPtr deck, const std::string& startKeywordName)
    {
        // find the first occurence of the section's start keyword
        size_t startKeywordIdx = 0;
        for (; startKeywordIdx<deck->size(); startKeywordIdx++) {
            if (deck->getKeyword(startKeywordIdx)->name() == startKeywordName)
                break;
        }

        if (startKeywordIdx >= deck->size())
            throw std::invalid_argument(std::string("Deck requires a '")+startKeywordName+"' section");

        // make sure that the section identifier is unique
        for (size_t j = startKeywordIdx + 1; j < deck->size(); j++)
            if (deck->getKeyword(j)->name() == startKeywordName)
                throw std::invalid_argument(std::string("Deck contains the '")+startKeywordName+"' section multiple times");

        // populate the section with keywords
        for (size_t curKeywordIdx = startKeywordIdx;
             curKeywordIdx < deck->size();
             curKeywordIdx++)
        {
            const std::string &keywordName = deck->getKeyword(curKeywordIdx)->name();
            if (curKeywordIdx > startKeywordIdx && isSectionDelimiter(keywordName))
                break;

            m_keywords.addKeyword(deck->getKeyword(curKeywordIdx));
        }
    }

    size_t Section::count(const std::string& keyword) const {
        return m_keywords.numKeywords( keyword );
    }

    const std::string& Section::name() const {
        return m_name;
    }

    bool Section::hasKeyword( const std::string& keyword ) const {
        return m_keywords.hasKeyword(keyword);
    }

    std::vector<DeckKeywordPtr>::iterator Section::begin() {
        return m_keywords.begin();
    }

    std::vector<DeckKeywordPtr>::iterator Section::end() {
        return m_keywords.end();
    }

    DeckKeywordConstPtr Section::getKeyword(const std::string& keyword, size_t index) const {
        return m_keywords.getKeyword(keyword , index);
    }

    DeckKeywordConstPtr Section::getKeyword(const std::string& keyword) const {
        return m_keywords.getKeyword(keyword);
    }
    
    DeckKeywordConstPtr Section::getKeyword(size_t index) const {
        return m_keywords.getKeyword(index);
    }

    bool Section::checkSectionTopology(DeckConstPtr deck, std::ostream& os)
    {
        if (deck->size() == 0) {
            os << "empty decks are invalid\n";
            return false;
        }

        if (deck->getKeyword(0)->name() != "RUNSPEC") {
            os << "The first keyword of a valid deck must be RUNSPEC\n";
            return false;
        }

        std::string curSectionName = deck->getKeyword(0)->name();
        size_t curKwIdx = 1;
        for (; curKwIdx < deck->size(); ++curKwIdx) {
            const std::string& curKeywordName = deck->getKeyword(curKwIdx)->name();
            if (!isSectionDelimiter(curKeywordName))
                continue;

            if (curSectionName == "RUNSPEC") {
                if (curKeywordName != "GRID") {
                    os << "The RUNSPEC section must be followed by GRID instead of "
                       << curKeywordName << "\n";
                    return false;
                }

                curSectionName = curKeywordName;
            }
            else if (curSectionName == "GRID") {
                if (curKeywordName != "EDIT" && curKeywordName != "PROPS") {
                    os << "The GRID section must be followed by EDIT or PROPS instead of "
                       << curKeywordName << "\n";
                    return false;
                }

                curSectionName = curKeywordName;
            }
            else if (curSectionName == "EDIT") {
                if (curKeywordName != "PROPS") {
                    os << "The EDIT section must be followed by PROPS instead of "
                       << curKeywordName << "\n";
                    return false;
                }

                curSectionName = curKeywordName;
            }
            else if (curSectionName == "PROPS") {
                if (curKeywordName != "REGIONS" && curKeywordName != "SOLUTION") {
                    os << "The PROPS section must be followed by REGIONS or SOLUTION instead of "
                       << curKeywordName << "\n";
                    return false;
                }

                curSectionName = curKeywordName;
            }
            else if (curSectionName == "REGIONS") {
                if (curKeywordName != "SOLUTION") {
                    os << "The REGIONS section must be followed by SOLUTION instead of "
                       << curKeywordName << "\n";
                    return false;
                }

                curSectionName = curKeywordName;
            }
            else if (curSectionName == "SOLUTION") {
                if (curKeywordName != "SUMMARY" && curKeywordName != "SCHEDULE") {
                    os << "The SOLUTION section must be followed by SUMMARY or SCHEDULE instead of "
                       << curKeywordName << "\n";
                    return false;
                }

                curSectionName = curKeywordName;
            }
            else if (curSectionName == "SUMMARY") {
                if (curKeywordName != "SCHEDULE") {
                    os << "The SUMMARY section must be followed by SCHEDULE instead of "
                       << curKeywordName << "\n";
                    return false;
                }

                curSectionName = curKeywordName;
            }
            else if (curSectionName == "SCHEDULE") {
                // schedule is the last section, so every section delimiter after it is wrong...
                os << "The SCHEDULE section must be the last one ("
                   << curKeywordName << " specified after SCHEDULE)\n";
                return false;
            }
        }

        // SCHEDULE is the last section and it is mandatory, so make sure it is there
        if (curSectionName != "SCHEDULE") {
            os << "The last section of a valid deck must be SCHEDULE\n";
            return false;
        }

        return true;
    }

    bool Section::isSectionDelimiter(const std::string& keywordName) {
        static std::set<std::string> sectionDelimiters;
        if (sectionDelimiters.size() == 0) {
            sectionDelimiters.insert("RUNSPEC");
            sectionDelimiters.insert("GRID");
            sectionDelimiters.insert("EDIT");
            sectionDelimiters.insert("PROPS");
            sectionDelimiters.insert("REGIONS");
            sectionDelimiters.insert("SOLUTION");
            sectionDelimiters.insert("SUMMARY");
            sectionDelimiters.insert("SCHEDULE");
        }

        return sectionDelimiters.count(keywordName) > 0;
    }

    bool Section::hasSection(DeckConstPtr deck, const std::string& startKeywordName) {
        return deck->hasKeyword(startKeywordName);
    }
    
}
