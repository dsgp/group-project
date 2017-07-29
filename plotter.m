results # load results

ports = unique(data(:,1));
ports = [20.0] #XXX

for port = ports
	pdata = [];

	for i = 1:length(data)
		if data(i,1) == port
			pdata = vertcat(pdata, data(i,:));
		end
	end


	figure(1);

	subplot(2, 1, 1);
	hold on
	grid on
	semilogx(pdata(:,2)', pdata(:,3)', 'linewidth', 2) # credit errors
	semilogx(pdata(:,2)', pdata(:,4)', 'linewidth', 2) # parity errors
	semilogx(pdata(:,2)', pdata(:,5)', 'linewidth', 2) # escape errors
	semilogx(pdata(:,2)', pdata(:,6)', 'linewidth', 2) # disc   errors

	xlabel('BER')
	ylabel('Error Count')
	legend('credit', 'parity', 'escape', 'disconnect', 'location', 'NorthWest')

	subplot(2, 1, 2);
	hold on
	grid on
	semilogx(pdata(:,2)', pdata(:,9)', 'linewidth', 2) # TX data
	semilogx(pdata(:,2)', pdata(:,10)', 'linewidth', 2) # TX ctrl
	semilogx(pdata(:,2)', pdata(:,7)', 'linewidth', 2) # EOP
	semilogx(pdata(:,2)', pdata(:,8)', 'linewidth', 2) # EEP

	xlabel('BER')
	ylabel('Transfer Count')
	legend('data', 'control', 'EOP', 'EEP', 'location', 'NorthWest')


end












