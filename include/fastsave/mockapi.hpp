#include "streaming_headers.hpp"
#include "spdlog/spdlog.h"
unsigned long MOCK_BUFFER_SIZE = 0;
unsigned long MOCK_RECORD_LENGTH = 0;
unsigned static long long RECORD_COUNT = 0;
bool MockDataStreaming(
    void** pdata, 
    void** pheaders, 
    unsigned char channelMask, 
    unsigned int* sampleCount, 
    unsigned int* headerCount, 
    unsigned int* headerStatus
)
{
    short** data = (short**)pdata;
    StreamingHeader_t ** headers = (StreamingHeader_t**)pheaders;

    unsigned long SAMPLES_PER_BUFFER = MOCK_BUFFER_SIZE / sizeof(short);
    unsigned long RECORDS_PER_BUFFER = SAMPLES_PER_BUFFER / MOCK_RECORD_LENGTH;
    unsigned long TRUE_SAMPLES_PER_BUFFER = RECORDS_PER_BUFFER * MOCK_RECORD_LENGTH;

    for(int ch = 0; ch < 4; ch++)
    {
        sampleCount[ch] = 0;
        headerCount[ch] = 0;
        headerStatus[ch] = 1;
        if((1<<ch) & channelMask)
        {
            spdlog::debug("Mocking channel {} buffers.", ch);
            sampleCount[ch] = TRUE_SAMPLES_PER_BUFFER;
            headerCount[ch] = RECORDS_PER_BUFFER;
            for(unsigned int recordIndex = 0; recordIndex < RECORDS_PER_BUFFER; recordIndex++)
            {
                headers[ch][recordIndex] = StreamingHeader_t();
                headers[ch][recordIndex].RecordLength = MOCK_RECORD_LENGTH;
                headers[ch][recordIndex].Channel = ch;
                headers[ch][recordIndex].RecordNumber = RECORD_COUNT++;
            }
            for(unsigned int sampleIndex = 0; sampleIndex < TRUE_SAMPLES_PER_BUFFER; sampleIndex++)
            {
                data[ch][sampleIndex] = sampleIndex;
            }
        }
        
    }
    return true;
}