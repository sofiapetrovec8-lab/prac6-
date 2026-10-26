#include <iostream>
using namespace std;

const int MAX_N = 10000;

struct Stats {
    long long comparisons;
    long long swaps;
    long long heapifyCalls;
};

// Скидання лічильників операцій
void resetStats(Stats& s) {
    s.comparisons = 0;
    s.swaps = 0;
    s.heapifyCalls = 0;
}

// Виведення масиву
void printArray(const int a[], int n) {
    cout << "[";
    for (int i = 0; i < n; i++) {
        cout << a[i];
        if (i < n - 1) cout << ", ";
    }
    cout << "]";
}

// Копіювання елементів масиву
void copyArray(const int source[], int target[], int n) {
    for (int i = 0; i < n; i++) {
        target[i] = source[i];
    }
}

// Обмін двох значень і підрахунок операцій
void swapValues(int& a, int& b, Stats& s) {
    int temp = a;
    a = b;
    b = temp;
    s.swaps++;
}

// Перевірка властивості максимальної купи
int checkMaxHeap(const int a[], int n) {
    for (int i = 0; i < n / 2; i++) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;

        if (left < n && a[i] < a[left]) {
            return i;
        }

        if (right < n && a[i] < a[right]) {
            return i;
        }
    }

    return -1;
}

// Відновлення властивості максимальної купи
void siftDown(int a[], int n, int i, Stats& s) {
    s.heapifyCalls++;

    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n) {
        s.comparisons++;
        if (a[left] > a[largest]) {
            largest = left;
        }
    }

    if (right < n) {
        s.comparisons++;
        if (a[right] > a[largest]) {
            largest = right;
        }
    }

    if (largest != i) {
        swapValues(a[i], a[largest], s);
        siftDown(a, n, largest, s);
    }
}

// Побудова максимальної купи знизу вгору
void buildMaxHeap(int a[], int n, Stats& s, bool trace) {
    for (int i = n / 2 - 1; i >= 0; i--) {
        siftDown(a, n, i, s);

        // Виведення проміжного стану для невеликих масивів
        if (trace && n <= 20) {
            cout << "After processing node i=" << i << ": ";
            printArray(a, n);
            cout << " (active size " << n << ")\n";
        }
    }
}

// Перевірка, чи відсортований масив
bool isSorted(const int a[], int n) {
    for (int i = 1; i < n; i++) {
        if (a[i - 1] > a[i]) {
            return false;
        }
    }
    return true;
}

// Обчислення суми елементів масиву
long long sumArray(const int a[], int n) {
    long long sum = 0;

    for (int i = 0; i < n; i++) {
        sum += a[i];
    }

    return sum;
}

// Обчислення суми квадратів елементів масиву
long long sumSquares(const int a[], int n) {
    long long sum = 0;

    for (int i = 0; i < n; i++) {
        sum += 1LL * a[i] * a[i];
    }

    return sum;
}

// Перевірка складу масивів за сумою та сумою квадратів
bool sameContent(const int a[], const int b[], int n) {
    return sumArray(a, n) == sumArray(b, n) &&
        sumSquares(a, n) == sumSquares(b, n);
}

// Повне сортування масиву алгоритмом HeapSort
void heapSort(int a[], int n,
    Stats& buildStats,
    Stats& sortStats) {
    resetStats(buildStats);
    resetStats(sortStats);

    buildMaxHeap(a, n, buildStats, false);

    // Перенесення найбільшого елемента в кінець масиву
    for (int end = n - 1; end > 0; end--) {
        swapValues(a[0], a[end], sortStats);
        siftDown(a, end, 0, sortStats);
    }
}

// Перевірка автоматичного тесту сортування
bool testSort(const int input[], int n,
    const int expected[], int expectedN,
    int testNo) {
    int a[MAX_N];
    int original[MAX_N];

    copyArray(input, a, n);
    copyArray(input, original, n);

    Stats buildStats, sortStats;
    heapSort(a, n, buildStats, sortStats);

    bool ok = n == expectedN &&
        isSorted(a, n) &&
        sameContent(original, a, n);

    // Порівняння результату з очікуваним масивом
    for (int i = 0; i < n && ok; i++) {
        if (a[i] != expected[i]) {
            ok = false;
        }
    }

    cout << "Sorting test " << testNo << ": "
        << (ok ? "OK" : "ERROR") << "\n";

    return ok;
}

// Перевірка правильності побудови максимальної купи
bool testHeapBuild(const int input[], int n, int testNo) {
    int a[MAX_N];
    copyArray(input, a, n);

    Stats s;
    resetStats(s);
    buildMaxHeap(a, n, s, false);

    bool ok = checkMaxHeap(a, n) == -1;

    cout << "Heap build test " << testNo << ": "
        << (ok ? "OK" : "ERROR") << "\n";

    return ok;
}

// Формування масивів різних типів для експерименту
void makeData(int a[], int n, int type) {
    for (int i = 0; i < n; i++) {
        if (type == 0) {
            a[i] = i + 1;
        }
        else if (type == 1) {
            a[i] = n - i;
        }
        else if (type == 2) {
            a[i] = (i * 37 + 11) % (n + 17);
        }
        else {
            a[i] = (i * 7 + i / 3) % 10;
        }
    }
}

// Проведення порівняльного експерименту
void experiment() {
    const int sizes[] = { 100, 1000, 10000 };

    const char* types[] = {
        "ascending",
        "descending",
        "mixed",
        "duplicates"
    };

    cout << "\nCOMPARATIVE EXPERIMENT\n";

    cout << "n; type; build: comparisons, swaps, heapify calls;"
        << " total: comparisons, swaps, heapify calls\n";

    for (int si = 0; si < 3; si++) {
        int n = sizes[si];

        for (int type = 0; type < 4; type++) {
            int a[MAX_N];
            makeData(a, n, type);

            Stats buildStats, sortStats;
            heapSort(a, n, buildStats, sortStats);

            cout << n << "; " << types[type] << "; "
                << buildStats.comparisons << "; "
                << buildStats.swaps << "; "
                << buildStats.heapifyCalls << "; "
                << buildStats.comparisons + sortStats.comparisons << "; "
                << buildStats.swaps + sortStats.swaps << "; "
                << buildStats.heapifyCalls + sortStats.heapifyCalls
                << "\n";

            // Перевірка правильності сортування після експерименту
            if (!isSorted(a, n)) {
                cout << "EXPERIMENT ERROR!\n";
            }
        }
    }
}

int main() {
    cout << "PRACTICAL WORK 6. HEAPSORT\n\n";

    const int* t1 = nullptr;
    const int* e1 = nullptr;

    const int t2[] = { 7 };
    const int e2[] = { 7 };

    const int t3[] = { 1, 2 };
    const int e3[] = { 1, 2 };

    const int t4[] = { 2, 1 };
    const int e4[] = { 1, 2 };

    const int t5[] = { 1, 2, 3, 4, 5 };
    const int e5[] = { 1, 2, 3, 4, 5 };

    const int t6[] = { 5, 4, 3, 2, 1 };
    const int e6[] = { 1, 2, 3, 4, 5 };

    const int t7[] = { 4, 1, 5, 2, 3 };
    const int e7[] = { 1, 2, 3, 4, 5 };

    const int t8[] = { 3, 1, 3, 2, 1 };
    const int e8[] = { 1, 1, 2, 3, 3 };

    const int t9[] = { 0, -5, 8, -2, 4 };
    const int e9[] = { -5, -2, 0, 4, 8 };

    const int t10[] = { 6, 6, 6, 6 };
    const int e10[] = { 6, 6, 6, 6 };

    const int t11[] = { 9, -1, 9, 0, -1, 5 };
    const int e11[] = { -1, -1, 0, 5, 9, 9 };

    const int t12[] = { 10, 7, 8, 4, 2, 1, 3 };
    const int e12[] = { 1, 2, 3, 4, 7, 8, 10 };

    const int t13[] = { 12, 4, 7, 4, 0, -3 };
    const int e13[] = { -3, 0, 4, 4, 7, 12 };

    const int t14[] = { 15, 2, 11, 9, 2, 18, 6, 0 };
    const int e14[] = { 0, 2, 2, 6, 9, 11, 15, 18 };

    int passed = 0;

    passed += testSort(t1, 0, e1, 0, 1);
    passed += testSort(t2, 1, e2, 1, 2);
    passed += testSort(t3, 2, e3, 2, 3);
    passed += testSort(t4, 2, e4, 2, 4);
    passed += testSort(t5, 5, e5, 5, 5);
    passed += testSort(t6, 5, e6, 5, 6);
    passed += testSort(t7, 5, e7, 5, 7);
    passed += testSort(t8, 5, e8, 5, 8);
    passed += testSort(t9, 5, e9, 5, 9);
    passed += testSort(t10, 4, e10, 4, 10);
    passed += testSort(t11, 6, e11, 6, 11);
    passed += testSort(t12, 7, e12, 7, 12);
    passed += testSort(t13, 6, e13, 6, 13);
    passed += testSort(t14, 8, e14, 8, 14);

    cout << "Successful sorting tests: "
        << passed << "/14\n\n";

    cout << "AUTOMATIC HEAP BUILD TESTS\n";

    int buildPassed = 0;

    buildPassed += testHeapBuild(t5, 5, 1);
    buildPassed += testHeapBuild(t6, 5, 2);
    buildPassed += testHeapBuild(t7, 5, 3);
    buildPassed += testHeapBuild(t8, 5, 4);
    buildPassed += testHeapBuild(t9, 5, 5);
    buildPassed += testHeapBuild(t10, 4, 6);
    buildPassed += testHeapBuild(t11, 6, 7);
    buildPassed += testHeapBuild(t12, 7, 8);
    buildPassed += testHeapBuild(t13, 6, 9);
    buildPassed += testHeapBuild(t14, 8, 10);

    cout << "Successful heap builds: "
        << buildPassed << "/10\n\n";

    cout << "HEAP PROPERTY TESTS\n";

    const int h1[] = { 9, 7, 8, 1, 3, 2 };
    const int h2[] = { 10, 5, 9, 1, 2 };
    const int h3[] = { 6, 6, 5, 2, 1 };

    const int h4[] = { 1, 7, 8, 9 };
    const int h5[] = { 9, 3, 8, 7 };
    const int h6[] = { 8, 7, 9, 1 };

    const int* heaps[] = { h1, h2, h3, h4, h5, h6 };
    const int hn[] = { 6, 5, 5, 4, 4, 4 };

    int heapPassed = 0;

    for (int i = 0; i < 6; i++) {
        bool shouldBeHeap = i < 3;
        bool actual = checkMaxHeap(heaps[i], hn[i]) == -1;
        bool ok = actual == shouldBeHeap;

        cout << "Heap property test " << i + 1 << ": "
            << (ok ? "OK" : "ERROR") << "\n";

        heapPassed += ok;
    }

    cout << "Successful heap property tests: "
        << heapPassed << "/6\n\n";

    cout << "VARIANT 18\n";

    const int variant[] = {
        37, 13, 24, 40, 9, 30, 17, 34,
        11, 21, 38, 8, 28, 15, 23
    };

    const int n = 15;

    int a[MAX_N];
    int original[MAX_N];

    copyArray(variant, a, n);
    copyArray(variant, original, n);

    cout << "Initial array: ";
    printArray(a, n);
    cout << "\n";

    Stats buildStats, sortStats;
    resetStats(buildStats);
    resetStats(sortStats);

    buildMaxHeap(a, n, buildStats, true);

    cout << "Heap after building: ";
    printArray(a, n);
    cout << "\n";

    cout << "Max-heap property check: "
        << (checkMaxHeap(a, n) == -1 ? "OK" : "ERROR")
        << "\n";

    cout << "Build statistics: comparisons="
        << buildStats.comparisons
        << ", swaps=" << buildStats.swaps
        << ", heapify calls=" << buildStats.heapifyCalls << "\n";

    int shown = 0;

    // Виконання сортування з виведенням перших трьох вилучень
    for (int end = n - 1; end > 0; end--) {
        swapValues(a[0], a[end], sortStats);
        siftDown(a, end, 0, sortStats);

        if (shown < 3) {
            cout << "Extraction #" << shown + 1 << ": ";
            printArray(a, n);

            cout << " (active heap=" << end
                << ", sorted part=" << n - end
                << ")\n";

            shown++;
        }
    }

    cout << "Sorted array: ";
    printArray(a, n);
    cout << "\n";

    cout << "Sorting statistics: comparisons="
        << sortStats.comparisons
        << ", swaps=" << sortStats.swaps
        << ", heapify calls=" << sortStats.heapifyCalls << "\n";

    cout << "Total statistics: comparisons="
        << buildStats.comparisons + sortStats.comparisons
        << ", swaps=" << buildStats.swaps + sortStats.swaps
        << ", heapify calls="
        << buildStats.heapifyCalls + sortStats.heapifyCalls << "\n";

    cout << "Order and content check: "
        << (isSorted(a, n) && sameContent(original, a, n)
            ? "OK" : "ERROR") << "\n";

    experiment();

    return 0;
}