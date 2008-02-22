echo "aclocal..."
aclocal

echo "libtoolize..."
libtoolize --force --copy

echo "automake..."
automake --add-missing --copy

echo "autoconf..."
autoconf


