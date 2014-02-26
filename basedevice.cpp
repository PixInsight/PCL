/*******************************************************************************
  Copyright(c) 2011 Jasem Mutlaq. All rights reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Library General Public
 License version 2 as published by the Free Software Foundation.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Library General Public License for more details.

 You should have received a copy of the GNU Library General Public License
 along with this library; see the file COPYING.LIB.  If not, write to
 the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 Boston, MA 02110-1301, USA.
*******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#if defined(WIN32)
#define snprintf _snprintf
#else
#define HAVE_ZLIB
#include <zlib.h>
#endif

#include "basedevice.h"
#include "baseclient.h"
#include "indicom.h"
#include "base64.h"
#include "indiproperty.h"

INDI::BaseDevice::BaseDevice()
{
    mediator = NULL;
    lp = newLilXML();
    deviceID = new char[MAXINDIDEVICE];
    memset(deviceID, 0, MAXINDIDEVICE);

    char indidev[MAXINDIDEVICE];
    strncpy(indidev, "INDIDEV=", MAXINDIDEVICE);

    if (getenv("INDIDEV") != NULL)
    {
        strncpy(deviceID, getenv("INDIDEV"), MAXINDIDEVICE);
        putenv(indidev);
    }
}

INDI::BaseDevice::~BaseDevice()
{
    delLilXML (lp);
    while(!pAll.empty())
    {
      delete pAll.back();
      pAll.pop_back();
    }

    delete[] deviceID;
}

INumberVectorProperty * INDI::BaseDevice::getNumber(const char *name)
{
    INumberVectorProperty * nvp = NULL;

    nvp = static_cast<INumberVectorProperty *> (getRawProperty(name, INDI_NUMBER));

    return nvp;
}

ITextVectorProperty * INDI::BaseDevice::getText(const char *name)
{
    ITextVectorProperty * tvp = NULL;

    tvp = static_cast<ITextVectorProperty *> (getRawProperty(name, INDI_TEXT));

    return tvp;
}

ISwitchVectorProperty * INDI::BaseDevice::getSwitch(const char *name)
{
    ISwitchVectorProperty * svp = NULL;

    svp = static_cast<ISwitchVectorProperty *> (getRawProperty(name, INDI_SWITCH));

    return svp;
}

ILightVectorProperty * INDI::BaseDevice::getLight(const char *name)
{
    ILightVectorProperty * lvp = NULL;

    lvp = static_cast<ILightVectorProperty *> (getRawProperty(name, INDI_LIGHT));

    return lvp;
}

IBLOBVectorProperty * INDI::BaseDevice::getBLOB(const char *name)
{       
  IBLOBVectorProperty * bvp = NULL;

  bvp = static_cast<IBLOBVectorProperty *> (getRawProperty(name, INDI_BLOB));

  return bvp;
}


void * INDI::BaseDevice::getRawProperty(const char *name, INDI_TYPE type)
{
    INDI_TYPE pType;
    void *pPtr;
    bool pRegistered = false;

    std::vector<INDI::Property *>::iterator orderi;

    INumberVectorProperty *nvp;
    ITextVectorProperty *tvp;
    ISwitchVectorProperty *svp;
    ILightVectorProperty *lvp;
    IBLOBVectorProperty *bvp;

    for (orderi = pAll.begin(); orderi != pAll.end(); orderi++)
    {
        pType       = (*orderi)->getType();
        pPtr        = (*orderi)->getProperty();
        pRegistered = (*orderi)->getRegistered();

        if (type != INDI_UNKNOWN && pType != type)
            continue;

        switch (pType)
        {
        case INDI_NUMBER:
            nvp = static_cast<INumberVectorProperty *>(pPtr);
            if (nvp == NULL)
                continue;

            if (!strcmp(name, nvp->name) && pRegistered){
				return pPtr;
			}
             break;
        case INDI_TEXT:
             tvp = static_cast<ITextVectorProperty *>(pPtr);
             if (tvp == NULL)
                 continue;

             if (!strcmp(name, tvp->name)  && pRegistered){
                return pPtr;
			 }
             break;
        case INDI_SWITCH:
             svp = static_cast<ISwitchVectorProperty *>(pPtr);
             if (svp == NULL)
                 continue;

             //IDLog("Switch %s and aux value is now %d\n", svp->name, regStatus );
             if (!strcmp(name, svp->name) && pRegistered){
                 return pPtr;
			 }
             break;
        case INDI_LIGHT:
             lvp = static_cast<ILightVectorProperty *>(pPtr);
             if (lvp == NULL)
                 continue;

             if (!strcmp(name, lvp->name)  && pRegistered){
                 return pPtr;
			 }
             break;
        case INDI_BLOB:
             bvp = static_cast<IBLOBVectorProperty *>(pPtr);
             if (bvp == NULL)
                 continue;

             if (!strcmp(name, bvp->name) && pRegistered){
                 return pPtr;
			 }
             break;

        case INDI_UNKNOWN:
               break;

        }
    }
    return NULL;
}

INDI::Property * INDI::BaseDevice::getProperty(const char *name, INDI_TYPE type)
{
    INDI_TYPE pType;
    void *pPtr;
    bool pRegistered = false;

    std::vector<INDI::Property *>::iterator orderi;

    INumberVectorProperty *nvp;
    ITextVectorProperty *tvp;
    ISwitchVectorProperty *svp;
    ILightVectorProperty *lvp;
    IBLOBVectorProperty *bvp;

    for (orderi = pAll.begin(); orderi != pAll.end(); orderi++)
    {
        pType       = (*orderi)->getType();
        pPtr        = (*orderi)->getProperty();
        pRegistered = (*orderi)->getRegistered();

        if (type != INDI_UNKNOWN && pType != type)
            continue;

        switch (pType)
        {
        case INDI_NUMBER:
            nvp = static_cast<INumberVectorProperty *>(pPtr);
            if (nvp == NULL)
                continue;

            if (!strcmp(name, nvp->name) && pRegistered)
                return *orderi;
             break;
        case INDI_TEXT:
             tvp = static_cast<ITextVectorProperty *>(pPtr);
             if (tvp == NULL)
                 continue;

             if (!strcmp(name, tvp->name)  && pRegistered)
                return *orderi;
             break;
        case INDI_SWITCH:
             svp = static_cast<ISwitchVectorProperty *>(pPtr);
             if (svp == NULL)
                 continue;

             //IDLog("Switch %s and aux value is now %d\n", svp->name, regStatus );
             if (!strcmp(name, svp->name) && pRegistered)
                 return *orderi;
             break;
        case INDI_LIGHT:
             lvp = static_cast<ILightVectorProperty *>(pPtr);
             if (lvp == NULL)
                 continue;

             if (!strcmp(name, lvp->name)  && pRegistered)
                 return *orderi;
             break;
        case INDI_BLOB:
             bvp = static_cast<IBLOBVectorProperty *>(pPtr);
             if (bvp == NULL)
                 continue;

             if (!strcmp(name, bvp->name) && pRegistered)
                 return *orderi;
             break;

        case INDI_UNKNOWN:
               break;

        }

    }

    return NULL;
}

int INDI::BaseDevice::removeProperty(const char *name, char *errmsg)
{    
    std::vector<INDI::Property *>::iterator orderi;

    INDI_TYPE pType;
    void *pPtr;

    INumberVectorProperty *nvp;
    ITextVectorProperty *tvp;
    ISwitchVectorProperty *svp;
    ILightVectorProperty *lvp;
    IBLOBVectorProperty *bvp;

    for (orderi = pAll.begin(); orderi != pAll.end(); orderi++)
    {
        pType       = (*orderi)->getType();
        pPtr        = (*orderi)->getProperty();

        switch (pType)
        {
        case INDI_NUMBER:
            nvp = static_cast<INumberVectorProperty *>(pPtr);
            if (!strcmp(name, nvp->name))
            {
                (*orderi)->setRegistered(false);
                delete *orderi;
                orderi = pAll.erase(orderi);
                 return 0;
             }
             break;
        case INDI_TEXT:
             tvp = static_cast<ITextVectorProperty *>(pPtr);
             if (!strcmp(name, tvp->name))
             {
                  (*orderi)->setRegistered(false);
                 delete *orderi;
                 orderi = pAll.erase(orderi);
                  return 0;
              }
             break;
        case INDI_SWITCH:
             svp = static_cast<ISwitchVectorProperty *>(pPtr);
             if (!strcmp(name, svp->name))
             {
                 (*orderi)->setRegistered(false);
                 delete *orderi;
                 orderi = pAll.erase(orderi);
                  return 0;
              }
             break;
        case INDI_LIGHT:
             lvp = static_cast<ILightVectorProperty *>(pPtr);
             if (!strcmp(name, lvp->name))
             {
                 (*orderi)->setRegistered(false);
                 delete *orderi;
                 orderi = pAll.erase(orderi);
                 return 0;
              }
             break;
        case INDI_BLOB:
             bvp = static_cast<IBLOBVectorProperty *>(pPtr);
             if (!strcmp(name, bvp->name))
             {
                 (*orderi)->setRegistered(false);
                 delete *orderi;
                 orderi = pAll.erase(orderi);
                 return 0;
              }
             break;

        case INDI_UNKNOWN:
               break;

        }
    }

    snprintf(errmsg, MAXRBUF, "Error: Property %s not found in device %s.", name, deviceID);
    return INDI_PROPERTY_INVALID;
}

void INDI::BaseDevice::buildSkeleton(const char *filename)
{
    char errmsg[MAXRBUF];
    FILE *fp = NULL;
    XMLEle *root = NULL, *fproot = NULL;

    fp = fopen(filename, "r");

    if (fp == NULL)
    {
        IDLog("Unable to build skeleton. Error loading file %s: %s\n", filename, strerror(errno));
        return;
    }

    fproot = readXMLFile(fp, lp, errmsg);

    if (fproot == NULL)
    {
        IDLog("Unable to parse skeleton XML: %s", errmsg);
        return;
    }

    //prXMLEle(stderr, fproot, 0);

    for (root = nextXMLEle (fproot, 1); root != NULL; root = nextXMLEle (fproot, 0))
            buildProp(root, errmsg);

    /**************************************************************************/
}

int INDI::BaseDevice::buildProp(XMLEle *root, char *errmsg)
{
    IPerm perm;
    IPState state;
    XMLEle *ep = NULL;
    char *rtag, *rname, *rdev;
    double timeout=0;

    rtag = tagXMLEle(root);

    /* pull out device and name */
    if (crackDN (root, &rdev, &rname, errmsg) < 0){
		return -1;
	}

    if (!deviceID[0])
        strncpy(deviceID, rdev, MAXINDINAME);

    //if (getProperty(rname, type) != NULL)
    if (getProperty(rname) != NULL){
        return INDI::BaseClient::INDI_PROPERTY_DUPLICATED;
	}

    if (strcmp (rtag, "defLightVector") && crackIPerm(findXMLAttValu(root, "perm"), &perm) < 0)
    {
        IDLog("Error extracting %s permission (%s)\n", rname, findXMLAttValu(root, "perm"));
        return -1;
    }

    timeout = atoi(findXMLAttValu(root, "timeout"));

    if (crackIPState (findXMLAttValu(root, "state"), &state) < 0)
    {
        IDLog("Error extracting %s state (%s)\n", rname, findXMLAttValu(root, "state"));
        return -1;
    }


    if (!strcmp (rtag, "defNumberVector"))
    {
        INDI::Property *indiProp = new INDI::Property();
        INumberVectorProperty *nvp = new INumberVectorProperty;

        INumber *np = NULL;
        int n=0;

        strncpy(nvp->device, deviceID, MAXINDIDEVICE);
        strncpy(nvp->name, rname, MAXINDINAME);
        strncpy(nvp->label, findXMLAttValu(root, "label"), MAXINDILABEL);
        strncpy(nvp->group, findXMLAttValu(root, "group"), MAXINDIGROUP);

        nvp->p       = perm;
        nvp->s       = state;
        nvp->timeout = timeout;

    /* pull out each name/value pair */
    for (n = 0, ep = nextXMLEle(root,1); ep != NULL; ep = nextXMLEle(root,0), n++)
    {
        if (!strcmp (tagXMLEle(ep), "defNumber"))
        {
            np = (INumber *) realloc(np, (n+1) * sizeof(INumber));

            np[n].nvp = nvp;

            XMLAtt *na = findXMLAtt (ep, "name");

            if (na)
            {
                if (f_scansexa (pcdataXMLEle(ep), &(np[n].value)) < 0)
                    IDLog("%s: Bad format %s\n", rname, pcdataXMLEle(ep));
                else
                {

                    strncpy(np[n].name, valuXMLAtt(na), MAXINDINAME);

                    na = findXMLAtt (ep, "label");
                    if (na)
                      strncpy(np[n].label, valuXMLAtt(na),MAXINDILABEL);
                    na = findXMLAtt (ep, "format");
                    if (na)
                        strncpy(np[n].format, valuXMLAtt(na), MAXINDIFORMAT);

                   na = findXMLAtt (ep, "min");
                   if (na)
                       np[n].min = atof(valuXMLAtt(na));
                   na = findXMLAtt (ep, "max");
                   if (na)
                       np[n].max = atof(valuXMLAtt(na));
                   na = findXMLAtt (ep, "step");
                   if (na)
                       np[n].step = atof(valuXMLAtt(na));

                }
            }
        }
    }

    if (n > 0)
    {
        nvp->nnp = n;
        nvp->np  = np;

        indiProp->setBaseDevice(this);
        indiProp->setProperty(nvp);
        indiProp->setDynamic(true);
        indiProp->setType(INDI_NUMBER);

        pAll.push_back(indiProp);

        //IDLog("Adding number property %s to list.\n", nvp->name);
        if (mediator)
            mediator->newProperty(indiProp);
    }
    else
        IDLog("%s: newNumberVector with no valid members\n",rname);
  }
  else if (!strcmp (rtag, "defSwitchVector"))
  {
            INDI::Property *indiProp = new INDI::Property();
            ISwitchVectorProperty *svp = new ISwitchVectorProperty;

            ISwitch *sp = NULL;
            int n=0;

            strncpy(svp->device, deviceID, MAXINDIDEVICE);
            strncpy(svp->name, rname, MAXINDINAME);
            strncpy(svp->label, findXMLAttValu(root, "label"), MAXINDILABEL);
            strncpy(svp->group, findXMLAttValu(root, "group"), MAXINDIGROUP);

            if (crackISRule(findXMLAttValu(root, "rule"), (&svp->r)) < 0)
                svp->r = ISR_1OFMANY;

            svp->p       = perm;
            svp->s       = state;
            svp->timeout = timeout;


        /* pull out each name/value pair */
        for (n = 0, ep = nextXMLEle(root,1); ep != NULL; ep = nextXMLEle(root,0), n++)
        {
            if (!strcmp (tagXMLEle(ep), "defSwitch"))
            {
                sp = (ISwitch *) realloc(sp, (n+1) * sizeof(ISwitch));

                sp[n].svp = svp;

                XMLAtt *na = findXMLAtt (ep, "name");

                if (na)
                {
                    crackISState(pcdataXMLEle(ep), &(sp[n].s));
                    strncpy(sp[n].name, valuXMLAtt(na), MAXINDINAME);

                    na = findXMLAtt (ep, "label");
                    if (na)
                        strncpy(sp[n].label, valuXMLAtt(na),MAXINDILABEL);
                 }
            }
        }

        if (n > 0)
        {
            svp->nsp = n;
            svp->sp  = sp;

            indiProp->setBaseDevice(this);
            indiProp->setProperty(svp);
            indiProp->setDynamic(true);
            indiProp->setType(INDI_SWITCH);

            pAll.push_back(indiProp);
            //IDLog("Adding Switch property %s to list.\n", svp->name);
            if (mediator)
                mediator->newProperty(indiProp);
        }
        else
            IDLog("%s: newSwitchVector with no valid members\n",rname);
    }

else if (!strcmp (rtag, "defTextVector"))
    {

        INDI::Property *indiProp = new INDI::Property();
        ITextVectorProperty *tvp = new ITextVectorProperty;
        IText *tp = NULL;
        int n=0;

        strncpy(tvp->device, deviceID, MAXINDIDEVICE);
        strncpy(tvp->name, rname, MAXINDINAME);
        strncpy(tvp->label, findXMLAttValu(root, "label"), MAXINDILABEL);
        strncpy(tvp->group, findXMLAttValu(root, "group"), MAXINDIGROUP);

        tvp->p       = perm;
        tvp->s       = state;
        tvp->timeout = timeout;

    // pull out each name/value pair
    for (n = 0, ep = nextXMLEle(root,1); ep != NULL; ep = nextXMLEle(root,0), n++)
    {
        if (!strcmp (tagXMLEle(ep), "defText"))
        {
            tp = (IText *) realloc(tp, (n+1) * sizeof(IText));

            tp[n].tvp = tvp;

            XMLAtt *na = findXMLAtt (ep, "name");

            if (na)
            {
                tp[n].text = (char *) malloc( (pcdatalenXMLEle(ep)*sizeof(char)) + 1);
                strncpy(tp[n].text, pcdataXMLEle(ep), pcdatalenXMLEle(ep));
                tp[n].text[pcdatalenXMLEle(ep)] = '\0';
                strncpy(tp[n].name, valuXMLAtt(na), MAXINDINAME);

                na = findXMLAtt (ep, "label");
                if (na)
                    strncpy(tp[n].label, valuXMLAtt(na),MAXINDILABEL);
             }
        }
    }

    if (n > 0)
    {
        tvp->ntp = n;
        tvp->tp  = tp;

        indiProp->setBaseDevice(this);
        indiProp->setProperty(tvp);
        indiProp->setDynamic(true);
        indiProp->setType(INDI_TEXT);

        pAll.push_back(indiProp);

        //IDLog("Adding Text property %s to list with initial value of %s.\n", tvp->name, tvp->tp[0].text);
        if (mediator)
            mediator->newProperty(indiProp);
    }
    else
        IDLog("%s: newTextVector with no valid members\n",rname);
}
else if (!strcmp (rtag, "defLightVector"))
    {

        INDI::Property *indiProp = new INDI::Property();
        ILightVectorProperty *lvp = new ILightVectorProperty;
        ILight *lp = NULL;
        int n=0;

        strncpy(lvp->device, deviceID, MAXINDIDEVICE);
        strncpy(lvp->name, rname, MAXINDINAME);
        strncpy(lvp->label, findXMLAttValu(root, "label"), MAXINDILABEL);
        strncpy(lvp->group, findXMLAttValu(root, "group"), MAXINDIGROUP);

        lvp->s       = state;

    /* pull out each name/value pair */
    for (n = 0, ep = nextXMLEle(root,1); ep != NULL; ep = nextXMLEle(root,0), n++)
    {
        if (!strcmp (tagXMLEle(ep), "defLight"))
        {
            lp = (ILight *) realloc(lp, (n+1) * sizeof(ILight));

            lp[n].lvp = lvp;

            XMLAtt *na = findXMLAtt (ep, "name");

            if (na)
            {
                crackIPState(pcdataXMLEle(ep), &(lp[n].s));
                strncpy(lp[n].name, valuXMLAtt(na), MAXINDINAME);

                na = findXMLAtt (ep, "label");
                if (na)
                    strncpy(lp[n].label, valuXMLAtt(na),MAXINDILABEL);

             }
        }
    }

    if (n > 0)
    {
        lvp->nlp = n;
        lvp->lp  = lp;

        indiProp->setBaseDevice(this);
        indiProp->setProperty(lvp);
        indiProp->setDynamic(true);
        indiProp->setType(INDI_LIGHT);

        pAll.push_back(indiProp);

        //IDLog("Adding Light property %s to list.\n", lvp->name);
        if (mediator)
            mediator->newProperty(indiProp);
    }
    else
        IDLog("%s: newLightVector with no valid members\n",rname);
}
else if (!strcmp (rtag, "defBLOBVector"))
    {
        INDI::Property *indiProp = new INDI::Property();
        IBLOBVectorProperty *bvp = new IBLOBVectorProperty;
        IBLOB *bp = NULL;
        int n=0;

        strncpy(bvp->device, deviceID, MAXINDIDEVICE);
        strncpy(bvp->name, rname, MAXINDINAME);
        strncpy(bvp->label, findXMLAttValu(root, "label"), MAXINDILABEL);
        strncpy(bvp->group, findXMLAttValu(root, "group"), MAXINDIGROUP);

        bvp->s       = state;
        bvp->p       = perm;

    /* pull out each name/value pair */
    for (n = 0, ep = nextXMLEle(root,1); ep != NULL; ep = nextXMLEle(root,0), n++)
    {
        if (!strcmp (tagXMLEle(ep), "defBLOB"))
        {
            bp = (IBLOB *) realloc(bp, (n+1) * sizeof(IBLOB));

            bp[n].bvp = bvp;

            XMLAtt *na = findXMLAtt (ep, "name");

            if (na)
            {
                strncpy(bp[n].name, valuXMLAtt(na), MAXINDINAME);

                na = findXMLAtt (ep, "label");
                if (na)
                    strncpy(bp[n].label, valuXMLAtt(na),MAXINDILABEL);

                na = findXMLAtt (ep, "format");
                if (na)
                    strncpy(bp[n].label, valuXMLAtt(na),MAXINDIBLOBFMT);

                // Initialize everything to zero

                bp[n].blob    = NULL;
                bp[n].size    = 0;
                bp[n].bloblen = 0;
             }

        }
    }

    if (n > 0)
    {
        bvp->nbp = n;
        bvp->bp  = bp;

        indiProp->setBaseDevice(this);
        indiProp->setProperty(bvp);
        indiProp->setDynamic(true);
        indiProp->setType(INDI_BLOB);

        pAll.push_back(indiProp);
        //IDLog("Adding BLOB property %s to list.\n", bvp->name);
        if (mediator)
            mediator->newProperty(indiProp);
    }
    else
        IDLog("%s: newBLOBVector with no valid members\n",rname);
 }

return (0);

}

bool INDI::BaseDevice::isConnected()
{
    ISwitchVectorProperty *svp = getSwitch("CONNECTION");
    if (!svp)
        return false;

    ISwitch * sp = IUFindSwitch(svp, "CONNECT");

    if (!sp)
        return false;

    if (sp->s == ISS_ON)
        return true;
    else
        return false;

}

/*
 * return 0 if ok else -1 with reason in errmsg
 */
int INDI::BaseDevice::setValue (XMLEle *root, char * errmsg)
{
    XMLAtt *ap;
    XMLEle *ep;
    char *rtag, *name;
    double timeout;
    IPState state;
    bool stateSet=false, timeoutSet=false;

    rtag = tagXMLEle(root);

    ap = findXMLAtt (root, "name");
    if (!ap)
    {
        snprintf(errmsg, MAXRBUF, "INDI: <%s> unable to find name attribute", tagXMLEle(root));
        return (-1);
    }

    name = valuXMLAtt(ap);

    /* set overall property state, if any */
    ap = findXMLAtt (root, "state");
    if (ap)
    {
        if (crackIPState(valuXMLAtt(ap), &state) != 0)
        {
            snprintf(errmsg, MAXRBUF, "INDI: <%s> bogus state %s for %s", tagXMLEle(root),
                     valuXMLAtt(ap), name);
            return (-1);
        }

        stateSet = true;
    }

    /* allow changing the timeout */
    ap = findXMLAtt (root, "timeout");
    if (ap)
    {
        timeout = atof(valuXMLAtt(ap));
        timeoutSet = true;
    }

    checkMessage (root);

    if (!strcmp(rtag, "setNumberVector"))
    {
        INumberVectorProperty *nvp = getNumber(name);
        if (nvp == NULL)
        {
            snprintf(errmsg, MAXRBUF, "INDI: Could not find property %s in %s", name, deviceID);
            return -1;
        }

        if (stateSet)
            nvp->s = state;

        if (timeoutSet)
            nvp->timeout = timeout;

       for (ep = nextXMLEle (root, 1); ep != NULL; ep = nextXMLEle (root, 0))
        {
           INumber *np =  IUFindNumber(nvp, findXMLAttValu(ep, "name"));
           if (!np)
               continue;

          np->value = atof(pcdataXMLEle(ep));

          // Permit changing of min/max
          if (findXMLAtt(ep, "min"))
              np->min = atof(findXMLAttValu(ep, "min"));
          if (findXMLAtt(ep, "max"))
              np->max = atof(findXMLAttValu(ep, "max"));
       }

       if (mediator)
           mediator->newNumber(nvp);

       return 0;
    }
    else if (!strcmp(rtag, "setTextVector"))
    {
        ITextVectorProperty *tvp = getText(name);
        if (tvp == NULL)
            return -1;

        if (stateSet)
            tvp->s = state;

        if (timeoutSet)
            tvp->timeout = timeout;

       for (ep = nextXMLEle (root, 1); ep != NULL; ep = nextXMLEle (root, 0))
        {
           IText *tp =  IUFindText(tvp, findXMLAttValu(ep, "name"));
           if (!tp)
               continue;

          IUSaveText(tp, pcdataXMLEle(ep));
       }

       if (mediator)
           mediator->newText(tvp);

       return 0;
    }
    else if (!strcmp(rtag, "setSwitchVector"))
    {
        ISState swState;
        ISwitchVectorProperty *svp = getSwitch(name);
        if (svp == NULL)
            return -1;

        if (stateSet)
            svp->s = state;

        if (timeoutSet)
            svp->timeout = timeout;

       for (ep = nextXMLEle (root, 1); ep != NULL; ep = nextXMLEle (root, 0))
        {
           ISwitch *sp =  IUFindSwitch(svp, findXMLAttValu(ep, "name"));
           if (!sp)
               continue;

           if (crackISState(pcdataXMLEle(ep), &swState) == 0)
               sp->s = swState;
        }

       if (mediator)
           mediator->newSwitch(svp);

       return 0;
    }
    else if (!strcmp(rtag, "setLightVector"))
    {
        IPState lState;
        ILightVectorProperty *lvp = getLight(name);
        if (lvp == NULL)
            return -1;

        if (stateSet)
            lvp->s = state;

       for (ep = nextXMLEle (root, 1); ep != NULL; ep = nextXMLEle (root, 0))
        {
           ILight *lp =  IUFindLight(lvp, findXMLAttValu(ep, "name"));
           if (!lp)
               continue;

           if (crackIPState(pcdataXMLEle(ep), &lState) == 0)
               lp->s = lState;
        }

       if (mediator)
           mediator->newLight(lvp);

       return 0;

    }
    else if (!strcmp(rtag, "setBLOBVector"))
    {
        IBLOBVectorProperty *bvp = getBLOB(name);
        if (bvp == NULL)
            return -1;

        if (stateSet)
            bvp->s = state;

        if (timeoutSet)
            bvp->timeout = timeout;

        return setBLOB(bvp, root, errmsg);
    }

    snprintf(errmsg, MAXRBUF, "INDI: <%s> Unable to process tag", tagXMLEle(root));
    return -1;
}

/* Set BLOB vector. Process incoming data stream
 * Return 0 if okay, -1 if error
*/
int INDI::BaseDevice::setBLOB(IBLOBVectorProperty *bvp, XMLEle * root, char * errmsg)
{   
    IBLOB *blobEL;
    unsigned char * dataBuffer=NULL;
    XMLEle *ep;
    int n=0, r=0;
#if defined(HAVE_ZLIB)
    uLongf dataSize=0;
#endif
    /* pull out each name/BLOB pair, decode */
    for (n = 0, ep = nextXMLEle(root,1); ep; ep = nextXMLEle(root,0))
    {
        if (strcmp (tagXMLEle(ep), "oneBLOB") == 0)
        {
            XMLAtt *na = findXMLAtt (ep, "name");

            blobEL = IUFindBLOB(bvp, findXMLAttValu (ep, "name"));

            XMLAtt *fa = findXMLAtt (ep, "format");
            XMLAtt *sa = findXMLAtt (ep, "size");
            if (na && fa && sa)
            {

                blobEL->size = atoi(valuXMLAtt(sa));

                /* Blob size = 0 when only state changes */
                if (blobEL->size == 0)
                    continue;

                 blobEL->blob = (unsigned char *) realloc (blobEL->blob, 3*pcdatalenXMLEle(ep)/4);

                 blobEL->bloblen = from64tobits( static_cast<char *> (blobEL->blob), pcdataXMLEle(ep));

                 strncpy(blobEL->format, valuXMLAtt(fa), MAXINDIFORMAT);
#if defined(HAVE_ZLIB)
                    if (strstr(blobEL->format, ".z"))
                    {
                        blobEL->format[strlen(blobEL->format)-2] = '\0';
                        dataSize = blobEL->size * sizeof(unsigned char);
                        dataBuffer = (unsigned char *) malloc(dataSize);

                        if (dataBuffer == NULL)
                        {
                                strncpy(errmsg, "Unable to allocate memory for data buffer", MAXRBUF);
                                return (-1);
                        }

                        r = uncompress(dataBuffer, &dataSize, static_cast<unsigned char *> (blobEL->blob), (uLong) blobEL->bloblen);
                        if (r != Z_OK)
                        {
                            snprintf(errmsg, MAXRBUF, "INDI: %s.%s.%s compression error: %d", blobEL->bvp->device, blobEL->bvp->name, blobEL->name, r);
                            free (dataBuffer);
                            return -1;
                        }
                        blobEL->size = dataSize;
                        free(blobEL->blob);
                        blobEL->blob = dataBuffer;
                    }
#endif
                    if (mediator)
                        mediator->newBLOB(blobEL);
                }
            else
            {
                snprintf(errmsg, MAXRBUF, "INDI: %s.%s.%s No valid members.", blobEL->bvp->device, blobEL->bvp->name, blobEL->name);
                return -1;
            }

           }
    }

    return 0;

}

void INDI::BaseDevice::setDeviceName(const char *dev)
{
    strncpy(deviceID, dev, MAXINDINAME);
}

const char * INDI::BaseDevice::getDeviceName()
{
    return deviceID;
}

/* add message to queue
 * N.B. don't put carriage control in msg, we take care of that.
 */
void INDI::BaseDevice::checkMessage (XMLEle *root)
{
    XMLAtt *ap;
    ap = findXMLAtt(root, "message");

    if (ap)
        doMessage(root);
}

/* Store msg in queue */
void INDI::BaseDevice::doMessage (XMLEle *msg)
{
    XMLAtt *message;
    XMLAtt *time_stamp;

    // FIXME: delete the buffer in the destructor!
    char * msgBuffer = new char[MAXRBUF];

    /* prefix our timestamp if not with msg */
    time_stamp = findXMLAtt (msg, "timestamp");

    /* finally! the msg */
    message = findXMLAtt(msg, "message");
        if (!message) return;

    if (time_stamp)
        snprintf(msgBuffer, MAXRBUF, "%s: %s ", valuXMLAtt(time_stamp), valuXMLAtt(message));
    else
        snprintf(msgBuffer, MAXRBUF, "%s: %s ", timestamp(), valuXMLAtt(message));

    // Prepend to the log
   addMessage(msgBuffer);

}


void INDI::BaseDevice::addMessage(const char *msg)
{
    messageLog.push_back(msg);

    if (mediator)
        mediator->newMessage(this, messageLog.size()-1);
}

const char * INDI::BaseDevice::messageQueue(int index)
{
    if ((unsigned int)index >= messageLog.size())
        return NULL;

    return messageLog.at(index);

}

const char * INDI::BaseDevice::lastMessage()
{
    return messageLog.back();
}

void INDI::BaseDevice::registerProperty(void *p, INDI_TYPE type)
{
    INDI::Property *pContainer;

    if (type == INDI_NUMBER)
    {
        INumberVectorProperty *nvp = static_cast<INumberVectorProperty *> (p);
        if ( (pContainer = getProperty(nvp->name, INDI_NUMBER)) != NULL)
        {
            pContainer->setRegistered(true);
            return;
        }

        pContainer = new INDI::Property();
        pContainer->setProperty(p);
        pContainer->setType(type);

        pAll.push_back(pContainer);

    }
    else if (type == INDI_TEXT)
    {
       ITextVectorProperty *tvp = static_cast<ITextVectorProperty *> (p);

       if ( (pContainer = getProperty(tvp->name, INDI_TEXT)) != NULL)
       {
           pContainer->setRegistered(true);
           return;
       }

       pContainer = new INDI::Property();
       pContainer->setProperty(p);
       pContainer->setType(type);

       pAll.push_back(pContainer);


   }
    else if (type == INDI_SWITCH)
    {
       ISwitchVectorProperty *svp = static_cast<ISwitchVectorProperty *> (p);

       if ( (pContainer = getProperty(svp->name, INDI_SWITCH)) != NULL)
       {
           pContainer->setRegistered(true);
           return;
       }

       pContainer = new INDI::Property();
       pContainer->setProperty(p);
       pContainer->setType(type);

       pAll.push_back(pContainer);

    }
    else if (type == INDI_LIGHT)
    {
       ILightVectorProperty *lvp = static_cast<ILightVectorProperty *> (p);

       if ( (pContainer = getProperty(lvp->name, INDI_LIGHT)) != NULL)
       {
           pContainer->setRegistered(true);
           return;
       }

       pContainer = new INDI::Property();
       pContainer->setProperty(p);
       pContainer->setType(type);

       pAll.push_back(pContainer);
   }
    else if (type == INDI_BLOB)
    {
       IBLOBVectorProperty *bvp = static_cast<IBLOBVectorProperty *> (p);

       if ( (pContainer = getProperty(bvp->name, INDI_BLOB)) != NULL)
       {
           pContainer->setRegistered(true);
           return;
       }

       pContainer = new INDI::Property();
       pContainer->setProperty(p);
       pContainer->setType(type);

       pAll.push_back(pContainer);

    }
}

const char *INDI::BaseDevice::getDriverName()
{
    ITextVectorProperty *driverInfo = getText("DRIVER_INFO");

    if (driverInfo == NULL)
        return NULL;

    IText *driverName = IUFindText(driverInfo, "NAME");
    if (driverName)
        return driverName->text;

    return NULL;
}

const char *INDI::BaseDevice::getDriverExec()
{
    ITextVectorProperty *driverInfo = getText("DRIVER_INFO");

    if (driverInfo == NULL)
        return NULL;

    IText *driverExec = IUFindText(driverInfo, "EXEC");
    if (driverExec)
        return driverExec->text;

    return NULL;
}

