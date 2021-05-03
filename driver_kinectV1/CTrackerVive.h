#pragma once

#include "CEmulatedDevice.h"

class CTrackerVive final : public CEmulatedDevice
{
    size_t m_index;

    CTrackerVive(const CTrackerVive &that) = delete;
    CTrackerVive& operator=(const CTrackerVive &that) = delete;

    void SetupProperties() override;
public:
    explicit CTrackerVive(size_t f_index);
    ~CTrackerVive();
};
