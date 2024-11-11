#pragma once

#include <array>
#include <type_traits>
#include <cmath>

namespace dory::math
{
    using Dimensions4 = std::integral_constant<std::size_t, 4>;
    using Dimensions3 = std::integral_constant<std::size_t, 3>;
    using Dimensions2 = std::integral_constant<std::size_t, 2>;
    using Dimensions1 = std::integral_constant<std::size_t, 1>;

    template<std::size_t N>
    struct AbstractVector
    {
        static constexpr std::size_t Dimensions = N;
    };

    template<typename T, typename U>
    struct Vector
    {};

    template<typename T, std::size_t N>
    struct Vector<T, std::integral_constant<T, N>>: public AbstractVector<N>
    {
    };

    template<typename T>
    struct Vector<T, Dimensions1>: public AbstractVector<Dimensions1::value>
    {
        T x = {};

        Vector() = default;

        explicit Vector(T x): x(x)
        {}

        template<typename TVector>
        requires(TVector::Dimensions == Dimensions1::value)
        decltype(auto) operator+(const TVector& right)
        {
            return Vector{x + right.x};
        }

        template<typename TVector>
        requires(TVector::Dimensions == Dimensions1::value)
        decltype(auto) operator*(const TVector& right)
        {
            return x * right.x;
        }

        bool operator==(const Vector<T, Dimensions1>& other) const {
            return (x == other.x);
        }
    };

    template<typename T>
    struct Vector<T, Dimensions2>: public AbstractVector<Dimensions2::value>
    {
        T x = {};
        T y = {};

        Vector() = default;

        Vector(T x, T y): x(x), y(y)
        {}

        template<typename TVector>
        requires(TVector::Dimensions == Dimensions2::value)
        decltype(auto) operator+(const TVector& right)
        {
            return Vector{ x + right.x, y + right.y };
        }

        template<typename TVector>
        requires(TVector::Dimensions == Dimensions2::value)
        decltype(auto) operator*(const TVector& right)
        {
            return x * right.x + y * right.y;
        }

        bool operator==(const Vector<T, Dimensions2>& other) const {
            return (x == other.x) &&
                   (y == other.y);
        }
    };

    template<typename T>
    struct Vector<T, Dimensions3>: public AbstractVector<Dimensions3::value>
    {
        T x = {};
        T y = {};
        T z = {};

        Vector() = default;

        Vector(T x, T y, T z): x(x), y(y), z(z)
        {}

        template<typename TVector>
        requires(TVector::Dimensions == Dimensions3::value)
        decltype(auto) operator+(const TVector& right)
        {
            return Vector{ x + right.x, y + right.y, z + right.z };
        }

        template<typename TVector>
        requires(TVector::Dimensions == Dimensions3::value)
        decltype(auto) operator*(const TVector& right)
        {
            return x * right.x + y * right.y + z * right.z;
        }

        bool operator==(const Vector<T, Dimensions3>& other) const {
            return (x == other.x) &&
                   (y == other.y) &&
                   (z == other.z);
        }
    };

    template<typename T>
    struct Vector<T, Dimensions4>: public AbstractVector<Dimensions4::value>
    {
        T x = {};
        T y = {};
        T z = {};
        T w = {};

        Vector() = default;

        Vector(T x, T y, T z, T w): x(x), y(y), z(z), w(w)
        {}

        template<typename TVector>
        requires(TVector::Dimensions == Dimensions4::value)
        decltype(auto) operator+(const TVector& right)
        {
            return Vector{ x + right.x, y + right.y, z + right.z, w + right.w };
        }

        template<typename TVector>
        requires(TVector::Dimensions == Dimensions4::value)
        decltype(auto) operator*(const TVector& right)
        {
            return  Vector<T, Dimensions3>::x * right.x + Vector<T, Dimensions3>::y * right.y + Vector<T, Dimensions3>::z * right.z + w * right.w;
        }

        bool operator==(const Vector<T, Dimensions4>& other) const {
            return (x == other.x) &&
                   (y == other.y) &&
                   (z == other.z) &&
                   (w == other.w);
        }
    };

    using Vector1f = Vector<float, Dimensions1>;
    using Vector2f = Vector<float, Dimensions2>;
    using Vector3f = Vector<float, Dimensions3>;
    using Vector4f = Vector<float, Dimensions4>;

    struct EntryPosition
    {
        std::size_t row;
        std::size_t column;

        EntryPosition(std::size_t row, std::size_t column):
                row(row),
                column(column)
        {
        }
    };

    template<typename T, std::size_t RowsCount, std::size_t ColumnsCount>
    class GeneralMatrix
    {
    public:
        static constexpr std::size_t ColumnDimensions = RowsCount;
        static constexpr std::size_t RowDimensions = ColumnsCount;
        static constexpr std::size_t EntriesCount = RowsCount * ColumnsCount;

        using EntriesArray = std::array<T, EntriesCount>;
        EntriesArray entries;

        explicit GeneralMatrix(EntriesArray&& initEntries):
                entries(std::move(initEntries))
        {}

        GeneralMatrix() = default;

        GeneralMatrix(const GeneralMatrix& other)
        {
            entries = other.entries;
        }

        GeneralMatrix(const GeneralMatrix&& other) noexcept
        {
            entries = std::move(other.entries);
        }

        GeneralMatrix& operator=(GeneralMatrix&& other) noexcept
        {
            entries = std::move(other.entries);
            return *this;
        }

        GeneralMatrix& operator=(const GeneralMatrix& other)
        {
            if(other.entries.data() != entries.data())
            {
                entries = other.entries;
            }

            return *this;
        }

        void set(EntriesArray&& newEntries)
        {
            entries = std::move(newEntries);
        }

        bool isEqual(const GeneralMatrix& other)
        {
            for(int i = 0; i < EntriesCount; ++i)
            {
                if(entries[i] != other.entries[i])
                {
                    return false;
                }
            }

            return true;
        }
    };

    template<typename T, typename R, typename C>
    class Matrix;

    template<typename T, std::size_t RowsCount, std::size_t ColumnsCount>
    class Matrix<T, std::integral_constant<std::size_t, RowsCount>, std::integral_constant<std::size_t, ColumnsCount>>
    {};

    template<typename T>
    class Matrix<T, Dimensions4, Dimensions4> : public GeneralMatrix<T, Dimensions4::value, Dimensions4::value>
    {
    public:
        using ParentType = GeneralMatrix<T, Dimensions4::value, Dimensions4::value>;

        explicit Matrix(ParentType::EntriesArray&& initEntries):
                ParentType(std::move(initEntries))
        {}

        Matrix() = default;

        Matrix(const Matrix& other):
                ParentType(other)
        {}

        Matrix(const Matrix&& other) noexcept:
                ParentType(std::move(other))
        {}

        Matrix& operator=(Matrix&& other) noexcept
        {
            ParentType::entries = std::move(other.entries);
            return *this;
        }

        Matrix& operator=(const Matrix& other)
        {
            if(other.entries.data() != ParentType::entries.data())
            {
                ParentType::entries = other.entries;
            }
            return *this;
        }

        EntryPosition getPositionByIndex(const std::size_t index)
        {
            const auto row = index / Dimensions4::value;
            return {row, index - row * Dimensions4::value};
        }

        bool isDiagonalEntry(const EntryPosition& position)
        {
            return position.row == position.column;
        }

        void toIdentity()
        {
            const T f1 = 1;
            const T f0 = 0;

            auto& entries = ParentType::entries;

            entries[0] = f1; entries[1] = f0; entries[2] = f0; entries[3] = f0;
            entries[4] = f0; entries[5] = f1; entries[6] = f0; entries[7] = f0;
            entries[8] = f0; entries[9] = f0; entries[10] = f1; entries[11] = f0;
            entries[12] = f0; entries[13] = f0; entries[14] = f0; entries[15] = f1;
        }

        template<typename TMatrix>
        requires(std::is_base_of_v<GeneralMatrix<T, Dimensions4::value, Dimensions4::value>, TMatrix>
                && TMatrix::ColumnDimensions == Dimensions4::value && TMatrix::RowDimensions == Dimensions4::value)
        decltype(auto) multiply(const TMatrix& right)
        {
            const auto& lhs = ParentType::entries;
            const auto& rhs = right.entries;

            auto a00 = lhs[0], a01 = lhs[1], a02 = lhs[2], a03 = lhs[3],
                    a10 = lhs[4], a11 = lhs[5], a12 = lhs[6], a13 = lhs[7],
                    a20 = lhs[8], a21 = lhs[9], a22 = lhs[10], a23 = lhs[11],
                    a30 = lhs[12], a31 = lhs[13], a32 = lhs[14], a33 = lhs[15];

            auto b00 = rhs[0], b01 = rhs[1], b02 = rhs[2], b03 = rhs[3],
                    b10 = rhs[4], b11 = rhs[5], b12 = rhs[6], b13 = rhs[7],
                    b20 = rhs[8], b21 = rhs[9], b22 = rhs[10], b23 = rhs[11],
                    b30 = rhs[12], b31 = rhs[13], b32 = rhs[14], b33 = rhs[15];

            auto m00 = a00 * b00 + a01 * b10 + a02 * b20 + a03 * b30;
            auto m10 = a10 * b00 + a11 * b10 + a12 * b20 + a13 * b30;
            auto m20 = a20 * b00 + a21 * b10 + a22 * b20 + a23 * b30;
            auto m30 = a30 * b00 + a31 * b10 + a32 * b20 + a33 * b30;

            auto m01 = a00 * b01 + a01 * b11 + a02 * b21 + a03 * b31;
            auto m11 = a10 * b01 + a11 * b11 + a12 * b21 + a13 * b31;
            auto m21 = a20 * b01 + a21 * b11 + a22 * b21 + a23 * b31;
            auto m31 = a30 * b01 + a31 * b11 + a32 * b21 + a33 * b31;

            auto m02 = a00 * b02 + a01 * b12 + a02 * b22 + a03 * b32;
            auto m12 = a10 * b02 + a11 * b12 + a12 * b22 + a13 * b32;
            auto m22 = a20 * b02 + a21 * b12 + a22 * b22 + a23 * b32;
            auto m32 = a30 * b02 + a31 * b12 + a32 * b22 + a33 * b32;

            auto m03 = a00 * b03 + a01 * b13 + a02 * b23 + a03 * b33;
            auto m13 = a10 * b03 + a11 * b13 + a12 * b23 + a13 * b33;
            auto m23 = a20 * b03 + a21 * b13 + a22 * b23 + a23 * b33;
            auto m33 = a30 * b03 + a31 * b13 + a32 * b23 + a33 * b33;

            auto result = Matrix<T, Dimensions4, Dimensions4>{};

            auto& r = result.entries;
            r[0] = m00; r[1] = m01; r[2] = m02; r[3] = m03;
            r[4] = m10; r[5] = m11; r[6] = m12; r[7] = m13;
            r[8] = m20; r[9] = m21; r[10] = m22; r[11] = m23;
            r[12] = m30; r[13] = m31; r[14] = m32; r[15] = m33;

            return result;
        }

        template<typename TVector>
        requires(std::is_base_of_v<AbstractVector<Dimensions3::value>, TVector> && TVector::Dimensions == Dimensions3::value)
        decltype(auto) multiply(const TVector& vector)
        {
            const auto& entries = ParentType::entries;

            auto x = (vector.x * entries[0]) + (vector.y * entries[1]) + (vector.z * entries[2]) + entries[3];
            auto y = (vector.x * entries[4]) + (vector.y * entries[5]) + (vector.z * entries[6]) + entries[7];
            auto z = (vector.x * entries[8]) + (vector.y * entries[9]) + (vector.z * entries[10]) + entries[11];

            return TVector{x, y, z};
        }

        template<typename TVector>
        requires(std::is_base_of_v<AbstractVector<Dimensions3::value>, TVector> && TVector::Dimensions == Dimensions3::value)
        decltype(auto) operator*(const TVector& vector)
        {
            return multiply(vector);
        }

        template<typename TVector>
        requires(std::is_base_of_v<AbstractVector<Dimensions4::value>, TVector> && TVector::Dimensions == Dimensions4::value)
        decltype(auto) multiply(const TVector& vector)
        {
            const auto& entries = ParentType::entries;

            auto x = (vector.x * entries[0]) + (vector.y * entries[1]) + (vector.z * entries[2]) + entries[3];
            auto y = (vector.x * entries[4]) + (vector.y * entries[5]) + (vector.z * entries[6]) + entries[7];
            auto z = (vector.x * entries[8]) + (vector.y * entries[9]) + (vector.z * entries[10]) + entries[11];
            auto w = (vector.x * entries[12]) + (vector.y * entries[13]) + (vector.z * entries[14]) + entries[15];

            return TVector{x, y, z, w};
        }

        template<typename TVector>
        requires(std::is_base_of_v<AbstractVector<Dimensions4::value>, TVector> && TVector::Dimensions == Dimensions4::value)
        decltype(auto) operator*(const TVector& vector)
        {
            return multiply(vector);
        }

        decltype(auto) getInverted()
        {
            const auto& m = ParentType::entries;

            auto a00 = m[0], a01 = m[1], a02 = m[2], a03 = m[3],
                    a10 = m[4], a11 = m[5], a12 = m[6], a13 = m[7],
                    a20 = m[8], a21 = m[9], a22 = m[10], a23 = m[11],
                    a30 = m[12], a31 = m[13], a32 = m[14], a33 = m[15],
                    b00 = a00 * a11 - a01 * a10,
                    b01 = a00 * a12 - a02 * a10,
                    b02 = a00 * a13 - a03 * a10,
                    b03 = a01 * a12 - a02 * a11,
                    b04 = a01 * a13 - a03 * a11,
                    b05 = a02 * a13 - a03 * a12,
                    b06 = a20 * a31 - a21 * a30,
                    b07 = a20 * a32 - a22 * a30,
                    b08 = a20 * a33 - a23 * a30,
                    b09 = a21 * a32 - a22 * a31,
                    b10 = a21 * a33 - a23 * a31,
                    b11 = a22 * a33 - a23 * a32,

                    det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;

            auto result = Matrix<T, Dimensions4, Dimensions4>{};

            if (det) {
                auto& resultData = result.entries;

                det = 1.0 / det;
                resultData[0] = (a11 * b11 - a12 * b10 + a13 * b09) * det;
                resultData[1] = (a02 * b10 - a01 * b11 - a03 * b09) * det;
                resultData[2] = (a31 * b05 - a32 * b04 + a33 * b03) * det;
                resultData[3] = (a22 * b04 - a21 * b05 - a23 * b03) * det;
                resultData[4] = (a12 * b08 - a10 * b11 - a13 * b07) * det;
                resultData[5] = (a00 * b11 - a02 * b08 + a03 * b07) * det;
                resultData[6] = (a32 * b02 - a30 * b05 - a33 * b01) * det;
                resultData[7] = (a20 * b05 - a22 * b02 + a23 * b01) * det;
                resultData[8] = (a10 * b10 - a11 * b08 + a13 * b06) * det;
                resultData[9] = (a01 * b08 - a00 * b10 - a03 * b06) * det;
                resultData[10] = (a30 * b04 - a31 * b02 + a33 * b00) * det;
                resultData[11] = (a21 * b02 - a20 * b04 - a23 * b00) * det;
                resultData[12] = (a11 * b07 - a10 * b09 - a12 * b06) * det;
                resultData[13] = (a00 * b09 - a01 * b07 + a02 * b06) * det;
                resultData[14] = (a31 * b01 - a30 * b03 - a32 * b00) * det;
                resultData[15] = (a20 * b03 - a21 * b01 + a22 * b00) * det;
            }

            return result;
        }

        template<typename TVector>
        requires(TVector::Dimensions == Dimensions3::value)
        void translate(const TVector& vector)
        {
            ParentType::entries[3] = vector.x;
            ParentType::entries[7] = vector.y;
            ParentType::entries[11] = vector.z;
        }

        template<typename TVector>
        requires(TVector::Dimensions == Dimensions3::value)
        void scale(const TVector& vector)
        {
            ParentType::entries[0] = vector.x;
            ParentType::entries[5] = vector.y;
            ParentType::entries[10] = vector.z;
        }

        template<typename TRadians>
        void rotateX(TRadians radians)
        {
            ParentType::entries[5] = std::cos(radians);
            ParentType::entries[6] = -std::sin(radians);
            ParentType::entries[9] = std::sin(radians);
            ParentType::entries[10] = std::cos(radians);
        }

        template<typename TRadians>
        void rotateY(TRadians radians)
        {
            ParentType::entries[0] = std::cos(radians);
            ParentType::entries[2] = std::sin(radians);
            ParentType::entries[8] = -std::sin(radians);
            ParentType::entries[10] = std::cos(radians);
        }

        template<typename TRadians>
        void rotateZ(TRadians radians)
        {
            ParentType::entries[0] = std::cos(radians);
            ParentType::entries[1] = -std::sin(radians);
            ParentType::entries[4] = std::sin(radians);
            ParentType::entries[5] = std::cos(radians);
        }
    };

    using Matrix4x4f = Matrix<float, Dimensions4, Dimensions4>;
    using Matrix4x4d = Matrix<double, Dimensions4, Dimensions4>;
    using Matrix4x4i = Matrix<int, Dimensions4, Dimensions4>;
}