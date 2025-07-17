#pragma once

static bool almostEqual(double a, double b, double epsilon = 1e-7) {
    return qAbs(a - b) <= epsilon * qMax(qAbs(a), qAbs(b));
}