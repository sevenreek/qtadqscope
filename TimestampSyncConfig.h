#ifndef TIMESTAMPSYNCCONFIG_H
#define TIMESTAMPSYNCCONFIG_H
class TimestampSyncConfig {
public:
    TimestampSyncConfig() {};
    TimestampSyncConfig(int source, bool syncOnMultiple, bool rearmOnStart)
        : source(source), syncOnMultipleEvents(syncOnMultiple), rearmOnAcquisitionStart(rearmOnStart) {};
    int source = 0;
    bool syncOnMultipleEvents = false;
    bool rearmOnAcquisitionStart = false;
};

#endif // TIMESTAMPSYNCCONFIG_H
