## Motywacja
pthread_mutex_t i std::mutex zajmują po 40 bajtów. Można sobie wyobrazić sytuację, gdy obiektów które chcielibyśmy chronić muteksami jest bardzo, bardzo dużo - wtedy każdy bajt się liczy.


Przykładem, jak można oszczędzić pamięć jest "turnstile". To koncepcja używana w NetBSD, Solarisie i pewnie jeszcze kilku innych systemach operacyjnych. Zasadza się ona na tym, że żaden wątek nie śpi na raz na więcej niż jednym muteksie. W tej koncepcji mutex staje się wydmuszką, która nie umożliwia zasypiania. Jeżeli o jeden mutex współzawodniczy więcej wątków i któryś należy wstrzymać, to dopiero wtedy przydzielamy muteksowi prawdziwy stan (który w tej terminologii nazywa się "turnstile"). Mutex będzie mieć przydzielony "turnstile" tylko tak długo jak, jakiś wątek będzie potrzebował na nim spać. Tym sposobem, na raz co najwyżej tyle muteksów ma przydzielony "turnstile", ile jest wątków. Można zaimplementować globalną pulę, z której mutex pobiera "turnstile", gdy ktoś usiłuje na nim usnąć i oddaje, gdy nikt już nie chce spać.

W internecie można znaleźć więcej informacji o tym, jak to działa w wymienionych systemach operacyjnych, lub można się domyślić.

## Zadanie
Zadanie polega na zaimplementowaniu klasy "Mutex", która będzie zajmować nie więcej niż 8 bajtów, o sygnaturze takiej jak w pliku "turnstile.h". Ilość użytej pamięci (poza samymi obiektami "Mutex") nie powinna być zależna od liczby obiektów typu Mutex, może być natomiast zależna od liczby wątków w programie.


Uprzedzając pytania, nie należy używać bezpośrednio futeksów ani wstawek asemblerowych. Do dyspozycji są std::mutex, std::condition_variable oraz std::atomic*.
