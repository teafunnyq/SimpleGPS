//
//  SegmentMapper.cpp
//  maps
//
//  Created by Tiffany Kyu on 3/10/17.
//  Copyright © 2017 Tiffany Kyu. All rights reserved.
//

#include "provided.h"
#include "MyMap.h"
#include <vector>

#include "support.h"

#include <string>
#include <iostream>
using namespace std;

class SegmentMapperImpl
{
public:
    SegmentMapperImpl();
    ~SegmentMapperImpl();
    void init(const MapLoader& ml);
    vector<StreetSegment> getSegments(const GeoCoord& gc) const;
    
private:
    //MyMap<double, MyMap<double,vector<StreetSegment>> * > lat2lon;
    MyMap<GeoCoord, vector<StreetSegment>> geo2street;
    void add2Map(GeoCoord ins, StreetSegment seg);
};

SegmentMapperImpl::SegmentMapperImpl()
{}

SegmentMapperImpl::~SegmentMapperImpl()
{}

void SegmentMapperImpl::add2Map(GeoCoord ins, StreetSegment seg)
{
    /*
    vector<StreetSegment> add;
    MyMap<double,vector<StreetSegment>>** temp = lat2lon.find(ins.latitude);
    
    if( temp != nullptr)
    {
        vector<StreetSegment>* sHolder = (*temp)->find(ins.longitude);
        if( sHolder != nullptr)
            sHolder->push_back(seg);
        else
        {
            add.push_back(seg);
            (*temp)->associate(ins.longitude,add);
        }
    }
    else
    {
        MyMap<double,vector<StreetSegment>>* addMap = new MyMap<double,vector<StreetSegment>>;
        add.push_back(seg);
        addMap->associate(ins.longitude, add);
        lat2lon.associate(ins.latitude,addMap);
    }
     */
    vector<StreetSegment> add;
    vector<StreetSegment>* temp = geo2street.find(ins); //See if GeoCoord already there
    
    if( temp != nullptr)    //If something already there
        temp->push_back(seg);   //Add seg to existing vector
    else    //Otherwise
    {
        add.push_back(seg); //Add segment to new vector
        geo2street.associate(ins,add);  //Associate vector and GeoCoord
    }
}

void SegmentMapperImpl::init(const MapLoader& ml)
{
    for(int i = 0; i < ml.getNumSegments(); i++)    //Go through each segment
    {
        StreetSegment seg;
        ml.getSegment(i, seg);  //Get segment info
        
        add2Map(seg.segment.start, seg);    //Add starting GeoCoord to BT
        add2Map(seg.segment.end, seg);  //Add ending GeoCoord to BT
        
        for(int j = 0; j < seg.attractions.size(); j++) //Go through each attraction of segment
            add2Map(seg.attractions[j].geocoordinates, seg);    //Add attraction GC to map
    }
}

vector<StreetSegment> SegmentMapperImpl::getSegments(const GeoCoord& gc) const
{
    /*
    MyMap<double,vector<StreetSegment>>* const *hold = lat2lon.find(gc.latitude);
    const vector<StreetSegment>* hold2 = (*hold)->find(gc.longitude);
    
    return *hold2;
     */
    vector<StreetSegment> empty;
    const vector<StreetSegment>* temp = geo2street.find(gc);    //Find GeoCoord in BT
    
    return *temp;
}
/*
bool operator <(const GeoCoord &c1, const GeoCoord &c2) //< overload of GC
{
    if(c1.latitude < c2.latitude)
        return true;
    return false;
}


bool operator ==(const GeoCoord &c1, const GeoCoord &c2)    //== overload of GC
{
    if(c1.latitude == c2.latitude && c1.longitude == c2.longitude)
        return true;
    return false;
}
*/
//******************** SegmentMapper functions ********************************

// These functions simply delegate to SegmentMapperImpl's functions.
// You probably don't want to change any of this code.

SegmentMapper::SegmentMapper()
{
    m_impl = new SegmentMapperImpl;
}

SegmentMapper::~SegmentMapper()
{
    delete m_impl;
}

void SegmentMapper::init(const MapLoader& ml)
{
    m_impl->init(ml);
}

vector<StreetSegment> SegmentMapper::getSegments(const GeoCoord& gc) const
{
    return m_impl->getSegments(gc);
}

