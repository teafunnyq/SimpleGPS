//
//  AttractionMapper.cpp
//  maps
//
//  Created by Tiffany Kyu on 3/10/17.
//  Copyright © 2017 Tiffany Kyu. All rights reserved.
//

#include "provided.h"
#include "MyMap.h"
#include <cctype>
#include <string>
using namespace std;

class AttractionMapperImpl
{
public:
    AttractionMapperImpl();
    ~AttractionMapperImpl();
    void init(const MapLoader& ml);
    bool getGeoCoord(string attraction, GeoCoord& gc) const;
    
private:
    MyMap<string, GeoCoord> att2seg;
};

AttractionMapperImpl::AttractionMapperImpl()
{}

AttractionMapperImpl::~AttractionMapperImpl()
{}

void AttractionMapperImpl::init(const MapLoader& ml)
{
    for(int i = 0; i < ml.getNumSegments(); i++)    //Go through each segment
    {
        StreetSegment streetSeg;
        ml.getSegment(i, streetSeg);    //Get info of each segment
        for(int j = 0; j < streetSeg.attractions.size(); j++)   //Go through each attraction of each segment
        {
            string low = streetSeg.attractions[j].name;
            for(int k = 0; k < low.size(); k++) //Make name of each attraction all lowercase
                low[k] = tolower(low[k]);
            att2seg.associate(low, streetSeg.attractions[j].geocoordinates);    //Add to BT
        }
    }
}

bool AttractionMapperImpl::getGeoCoord(string attraction, GeoCoord& gc) const
{
    string low = "";
    
    for(int i = 0; i < attraction.size(); i++)  //Make attraction name all lowercase
        low += tolower(attraction[i]);
    
    const GeoCoord* temp = att2seg.find(low);   //Look for attraction name in BT
    
    if (temp != nullptr)    //If you found something, add it to gc and return true
    {
        gc = *temp;
        return true;
    }
    
    return false;
}

//******************** AttractionMapper functions *****************************

// These functions simply delegate to AttractionMapperImpl's functions.
// You probably don't want to change any of this code.

AttractionMapper::AttractionMapper()
{
    m_impl = new AttractionMapperImpl;
}

AttractionMapper::~AttractionMapper()
{
    delete m_impl;
}

void AttractionMapper::init(const MapLoader& ml)
{
    m_impl->init(ml);
}

bool AttractionMapper::getGeoCoord(string attraction, GeoCoord& gc) const
{
    return m_impl->getGeoCoord(attraction, gc);
}

