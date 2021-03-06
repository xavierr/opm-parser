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

/**
   This class implements a small container which holds the
   transmissibility mulitpliers for all the faces in the grid. The
   multipliers in this class are built up from the transmissibility
   modifier keywords:

      {MULTX , MULTX- , MULTY , MULTY- , MULTZ , MULTZ-, MULTFLT , MULTREGT}

*/
#ifndef TRANSMULT_HPP
#define TRANSMULT_HPP


#include <cstddef>
#include <map>
#include <memory>

#include <opm/parser/eclipse/EclipseState/Grid/FaceDir.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/GridProperty.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/GridProperties.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/FaultCollection.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/MULTREGTScanner.hpp>


namespace Opm {
    class TransMult {

    public:
        TransMult(size_t nx , size_t ny , size_t nz);
        double getMultiplier(size_t globalIndex, FaceDir::DirEnum faceDir) const;
        double getMultiplier(size_t i , size_t j , size_t k, FaceDir::DirEnum faceDir) const;
        bool hasDirectionProperty(FaceDir::DirEnum faceDir) const;
        std::shared_ptr<GridProperty<double> > getDirectionProperty(FaceDir::DirEnum faceDir);
        void applyMULT(std::shared_ptr<const GridProperty<double> > srcMultProp, FaceDir::DirEnum faceDir);
        void applyMULTFLT( std::shared_ptr<const FaultCollection> faults);
        void applyMULTREGT( std::shared_ptr<MULTREGTScanner> multregtScanner , std::shared_ptr<GridProperties<int> > regions);

    private:
        size_t getGlobalIndex(size_t i , size_t j , size_t k) const;
        void assertIJK(size_t i , size_t j , size_t k) const;
        double getMultiplier__(size_t globalIndex , FaceDir::DirEnum faceDir) const;
        void insertNewProperty(FaceDir::DirEnum faceDir);

        size_t m_nx , m_ny , m_nz;
        std::map<FaceDir::DirEnum , std::shared_ptr<GridProperty<double> > > m_trans;
        std::map<FaceDir::DirEnum , std::string> m_names;
    };

}

#endif
