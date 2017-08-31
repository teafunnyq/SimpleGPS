//
//  MapLoader.cpp
//  maps
//
//  Created by Tiffany Kyu on 3/10/17.
//  Copyright © 2017 Tiffany Kyu. All rights reserved.
//

#include "provided.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

class MapLoaderImpl
{
public:
    MapLoaderImpl();
    ~MapLoaderImpl();
    bool load(string mapFile);
    size_t getNumSegments() const;
    bool getSegment(size_t segNum, StreetSegment& seg) const;
    
private:
    int m_segs;
    vector<StreetSegment> streetSegs;
};

MapLoaderImpl::MapLoaderImpl()
:m_segs(0)
{}

MapLoaderImpl::~MapLoaderImpl()
{}

bool MapLoaderImpl::load(string mapFile)
{
    ifstream infile(mapFile);   //Load mapFile
    
    if (!infile)    //Can't load, so return false
        return false;
    
    string s;
    int linNum = 0, attNum = 0, numAtt = 0; //Line number of segment info, attraction element for segment info, and number of attractions for a segment
    
    //linNum code: Segment name(0), Segment coordinates(1), number of attractions(2), going through attractions(3)
    
    while(getline(infile,s))
    {
        if(linNum == 3 && attNum == numAtt) //Went through all attractions for a segment
        {
            attNum = 0; //Start over line number and attraction number
            linNum = 0;
            m_segs++;   //Finished one seg
        }
        
        if(linNum == 0) //Started new set of segment info
        {
            StreetSegment add;  //Create new street segment, add its name, then go to next line
            //streetSegs.push_back(*new StreetSegment);
            streetSegs.push_back(add);
            streetSegs[m_segs].streetName = s;
            linNum++;
        }
        else if(linNum == 1)    //Reading segment coordinates
        {
            string geoCoords[4];
            for( int i = 0; i < s.size(); i++)  //Remove all commas from string of coordinates
            {
                if(s[i] == ',')
                    s[i] = ' ';
            }
            
            istringstream iss(s);
            for(int j = 0; j < 4; j++)  //Add each coordinate to array
            {
                string sub;
                iss >> sub;
                geoCoords[j] = sub;
            }
            
            //Create start and end GeoCoord for GeoSegment of this segment
            GeoCoord start(geoCoords[0],geoCoords[1]), end(geoCoords[2], geoCoords[3]);
            GeoSegment add(start,end);
            streetSegs[m_segs].segment = add;
            linNum++;
        }
        else if(linNum == 2 && s == "0")    //Read number of attractions and there's none
        {
            m_segs++;   //Finished reading info of this segment
            linNum = 0; //Start over
            continue;
        }
        else if (linNum == 2)   //Read number of attractions and there's something there
        {
            linNum++;
            numAtt = stoi(s);   //Number of atrraction convert to int
            continue;
        }
        else if(linNum == 3)    //Reading each attraction
        {
            string attCoord[2];
            string attName = "", coordName = "";
            int count = 0;
            
            for(int k = 0; k < s.size(); k++)   //Go through the line
            {
                if(s[k] == '|') //Reach | separating the name and coorindate
                {
                    count++;    //Break
                    break;
                }
                attName += s[k];    //Otherwise it's char, so add it to attraction name
                count++;    //Count of where to start reading coordinates
            }
            
            Attraction add; //Create new attraction, add to vector, and put its name
            //streetSegs[m_segs].attractions.push_back(*new Attraction);
            streetSegs[m_segs].attractions.push_back(add);
            streetSegs[m_segs].attractions[attNum].name = attName;
            
            for(int j = count; j<s.size(); j++) //Go through coordinates in line
            {
                if( s[j] == ',')    //if it's a comma, skip it
                {
                    coordName += ' ';
                    continue;
                }
                
                coordName += s[j];  //Otherwise it's a number, decimal, or negative so add it to corodinate
            }
            
            istringstream iss(coordName);
            for(int l = 0; l < 2; l++)  //Add coordinate to string
            {
                string sub;
                iss >> sub;
                attCoord[l] = sub;
            }
            
            //Create GeoCoord with coordinates and add to atraction info
            GeoCoord att(attCoord[0], attCoord[1]);
            streetSegs[m_segs].attractions[attNum].geocoordinates = att;
            attNum++;   //Move onto next attraction element to add to vector
        }
        
    }
    return true;
}

size_t MapLoaderImpl::getNumSegments() const
{
    return m_segs;
}

bool MapLoaderImpl::getSegment(size_t segNum, StreetSegment &seg) const
{
    if(segNum >= m_segs)
        return false;
    
    seg = streetSegs[segNum];
    return true;
}

//******************** MapLoader functions ************************************

// These functions simply delegate to MapLoaderImpl's functions.
// You probably don't want to change any of this code.

MapLoader::MapLoader()
{
    m_impl = new MapLoaderImpl;
}

MapLoader::~MapLoader()
{
    delete m_impl;
}

bool MapLoader::load(string mapFile)
{
    return m_impl->load(mapFile);
}

size_t MapLoader::getNumSegments() const
{
    return m_impl->getNumSegments();
}

bool MapLoader::getSegment(size_t segNum, StreetSegment& seg) const
{
    return m_impl->getSegment(segNum, seg);
}

