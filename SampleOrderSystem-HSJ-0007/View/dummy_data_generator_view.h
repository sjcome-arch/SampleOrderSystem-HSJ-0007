#pragma once

#include "Controller/dummy_data_generator_types.h"

// 생성 개수 입력/결과 요약 출력 (design_phase_8.md 참조).
class DummyDataGeneratorView {
public:
    int promptSpecCount() const;
    int promptOrderCount() const;
    void showSummary(const DummyDataGenerationSummary& summary) const;
};
