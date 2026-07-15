#pragma once

#include <vector>

#include "Controller/release_types.h"

// 출고 가능 목록/출고 처리 결과 출력 (REQUIREMENT.md 5.7).
class ReleaseView {
public:
    void showReleasableList(const std::vector<ReleasableOrderRow>& rows) const;
    int promptSelection() const;
    void showInvalidSelection() const;
    void showReleaseResult(const ReleaseResult& result) const;
};
