#include "View/product_spec_view.h"

#include <iostream>
#include <sstream>
#include <string>

namespace {

// UTF-8 기준 터미널 표시 폭(한글 등 멀티바이트 문자는 2칸으로 계산)을 구해 열을 맞춘다.
size_t displayWidth(const std::string& text) {
    size_t width = 0;
    for (size_t i = 0; i < text.size();) {
        unsigned char c = static_cast<unsigned char>(text[i]);
        size_t bytes = 1;
        if ((c & 0xE0) == 0xC0) {
            bytes = 2;
        } else if ((c & 0xF0) == 0xE0) {
            bytes = 3;
        } else if ((c & 0xF8) == 0xF0) {
            bytes = 4;
        }
        width += (bytes == 1) ? 1 : 2;
        i += bytes;
    }
    return width;
}

template <typename T>
std::string toStr(const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

std::string padColumn(const std::string& text, size_t columnWidth) {
    size_t width = displayWidth(text);
    return text + std::string(width < columnWidth ? columnWidth - width : 1, ' ');
}

}  // namespace

int ProductSpecView::promptMenuChoice() const {
    std::cout << "\n[시료 관리]\n1. 등록\n2. 조회\n3. 검색\n0. 뒤로가기\n선택: ";
    int choice = 0;
    std::cin >> choice;
    return choice;
}

RegisterProductSpecInput ProductSpecView::promptRegisterInput() const {
    RegisterProductSpecInput input;
    std::cout << "시료 ID: ";
    std::cin >> input.productSpecId;
    std::cout << "시료명: ";
    std::cin >> input.name;
    std::cout << "평균 생산시간(분): ";
    std::cin >> input.avgProductionTime;
    std::cout << "수율(0~1): ";
    std::cin >> input.yield;
    std::cout << "초기 재고: ";
    std::cin >> input.stock;
    return input;
}

bool ProductSpecView::promptConfirm(const RegisterProductSpecInput& input) const {
    std::cout << "시료ID=" << input.productSpecId << ", 시료명=" << input.name
              << ", 평균생산시간=" << input.avgProductionTime << ", 수율=" << input.yield
              << ", 초기재고=" << input.stock << " 로 등록하시겠습니까? (Y/N): ";
    char answer = 'N';
    std::cin >> answer;
    return answer == 'Y' || answer == 'y';
}

void ProductSpecView::showRegisterResult(RegisterProductSpecResult result) const {
    switch (result) {
        case RegisterProductSpecResult::Success:
            std::cout << "시료가 등록되었습니다." << std::endl;
            break;
        case RegisterProductSpecResult::InvalidYield:
            std::cout << "수율은 0 초과 1 이하 값이어야 합니다. 다시 입력하세요." << std::endl;
            break;
        case RegisterProductSpecResult::InvalidProductionTime:
            std::cout << "평균 생산시간은 양수여야 합니다. 다시 입력하세요." << std::endl;
            break;
        case RegisterProductSpecResult::DuplicateId:
            std::cout << "이미 등록된 시료 ID입니다. 다시 입력하세요." << std::endl;
            break;
        case RegisterProductSpecResult::Cancelled:
            std::cout << "등록이 취소되었습니다." << std::endl;
            break;
    }
}

void ProductSpecView::showList(const std::vector<ProductSpec>& specs) const {
    if (specs.empty()) {
        std::cout << "등록된 시료가 없습니다." << std::endl;
        return;
    }
    std::cout << padColumn("ID", 10) << padColumn("시료명", 14) << padColumn("평균생산시간", 14)
               << padColumn("수율", 8) << padColumn("재고", 8) << padColumn("가용재고", 8) << std::endl;
    for (const auto& spec : specs) {
        std::cout << padColumn(spec.productSpecId, 10) << padColumn(spec.name, 14)
                   << padColumn(toStr(spec.avgProductionTime), 14) << padColumn(toStr(spec.yield), 8)
                   << padColumn(toStr(spec.stock), 8) << padColumn(toStr(spec.availableStock), 8) << std::endl;
    }
}

std::string ProductSpecView::promptSearchKeyword() const {
    std::cout << "검색어(시료명): ";
    std::string keyword;
    std::cin >> keyword;
    return keyword;
}

void ProductSpecView::showSearchResults(const std::vector<ProductSpec>& specs) const {
    if (specs.empty()) {
        std::cout << "검색 결과 없음" << std::endl;
        return;
    }
    showList(specs);
}
