#pragma once

using Opera = std::function<bool()>;

auto SafeParseInt(const std::string& text, int& result) -> bool;

auto RunOperation(const std::string& name, const Opera& f) -> bool;