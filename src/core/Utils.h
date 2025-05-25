#pragma once

using Opera = std::function<bool()>;

auto RunOperation(const std::string& name, const Opera& f) -> bool;
