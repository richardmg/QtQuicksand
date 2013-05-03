/*
 * Copyright (c) 2010 Nokia Corporation.
 */

#ifndef CCAMERAFILEOBSERVER_H
#define CCAMERAFILEOBSERVER_H

#include <e32base.h>
#include <e32std.h>
#include <e32property.h>

class MCameraFileObserver 
{
public:
    virtual void NewCameraFileL(const TFileName& aFileName) = 0;
};

class CCameraFileObserver : public CActive
{
public:
    static CCameraFileObserver* NewL(MCameraFileObserver& aObserver);
    static CCameraFileObserver* NewLC(MCameraFileObserver& aObserver);
    ~CCameraFileObserver();

private:
    CCameraFileObserver(MCameraFileObserver& aObserver);
    void ConstructL();

private:
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);

public:
    void Start();
    void GetLastFileL(TFileName& aFileName);

private:
    MCameraFileObserver& iObserver;
    RProperty iProperty;
};

#endif // CCAMERAFILEOBSERVER_H
