results # load results

# index
ERR_CREDIT   = 3;
ERR_PARITY   = 4;
ERR_ESCAPE   = 5;
ERR_DISC     = 6;
NUM_NET_DATA = 7;
NUM_NET_EOP  = 8;
NUM_NET_EEP  = 9;
NUM_DDL_DATA = 10;
NUM_DDL_CTRL = 11;
NUM_DDL_EOP  = 12;
NUM_DDL_EEP  = 13;
NUM_DDL_FCT  = 14;
NUM_DDL_ESC  = 15;

ports = unique(data(:,1));
ports = [20.0] #XXX

for port = ports
	pdata = [];

	for i = 1:length(data)
		if data(i,1) == port
			pdata = vertcat(pdata, data(i,:));
		end
	end

	figure(1, 'position', [5, 5, 800, 800]);

	hold off
	subplot(3, 1, 1);
	hold on
	semilogx(pdata(:,2)', pdata(:,ERR_CREDIT)', 'linewidth', 2)
	semilogx(pdata(:,2)', pdata(:,ERR_PARITY)', 'linewidth', 2)
	semilogx(pdata(:,2)', pdata(:,ERR_ESCAPE)', 'linewidth', 2)
	semilogx(pdata(:,2)', pdata(:,ERR_DISC)',   'linewidth', 2)

	set(gca, "linewidth", 1, "fontsize", 12)
	title('Errors v. Bit Error Rate', 'fontsize', 14)
	xlabel('BER [errors/cycle]', 'fontsize', 12)
	ylabel('Error Count', 'fontsize', 12)
	legend('credit', 'parity', 'escape', 'disconnect', 'location', 'NorthWest')
	grid on

	hold off
	subplot(3, 1, 2);
	hold on
	semilogx(pdata(:,2)', pdata(:,NUM_DDL_DATA)', 'linewidth', 2)
	semilogx(pdata(:,2)', pdata(:,NUM_DDL_CTRL)', 'linewidth', 2)
	semilogx(pdata(:,2)', pdata(:,NUM_DDL_EOP)', 'linewidth', 4, 'linestyle', '-')
	semilogx(pdata(:,2)', pdata(:,NUM_DDL_EEP)', 'linewidth', 2, 'linestyle', '-')
	semilogx(pdata(:,2)', pdata(:,NUM_DDL_FCT)', 'linewidth', 2, 'linestyle', '-')
	semilogx(pdata(:,2)', pdata(:,NUM_DDL_ESC)', 'linewidth', 2, 'linestyle', '-')

	set(gca, "linewidth", 1, "fontsize", 12)
	title('DLL Transfers v. Bit Error Rate', 'fontsize', 14)
	xlabel('BER [errors/cycle]', 'fontsize', 12)
	ylabel('Transfer Count', 'fontsize', 12)
	legend('data', 'ctrl', 'eop', 'eep', 'fct', 'esc', 'location', 'NorthWest')
	grid on

	hold off
	subplot(3, 1, 3);
	hold on
	semilogx(pdata(:,2)', pdata(:,NUM_NET_DATA)', 'linewidth', 2)
	semilogx(pdata(:,2)', pdata(:,NUM_NET_EOP)', 'linewidth', 2)
	semilogx(pdata(:,2)', pdata(:,NUM_NET_EEP)', 'linewidth', 2)

	set(gca, "linewidth", 1, "fontsize", 12)
	title('NET Transfers v. Bit Error Rate', 'fontsize', 14)
	xlabel('BER [errors/cycle]', 'fontsize', 12)
	ylabel('Transfer Count', 'fontsize', 12)
	legend('data', 'eop', 'eep', 'location', 'NorthWest')
	grid on
end











