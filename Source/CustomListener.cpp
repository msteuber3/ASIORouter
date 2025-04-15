class CustomListener {
public:
	struct Listener {
		virtual ~Listener();
		virtual void eventTriggered() = 0;
	};
};