# netcpp
This is my personal cpp library which tries to simple my daily work and makes C++ a little easy to use. It is really fun if you know how to use them. Below are some code segments to show the usage of some of the classes.

# StreamReader

        StreamReader r("/path/to/ini/file");

        while (true)
        {
            r.skipWhitespace();
            if (r.peek() < 0) break;
            if (r.peek() == '#') { r.moveToNextLine(); continue; }

            string section = r.readQuoted("[]");

            while (true) // read keys under this secsion
            {
                r.skipWhitespace();

                int first = r.peek();
                if (first == '[' || first < 0) break;
                
                if (first == '#' || first == ';' || first < 0)
                {
                    r.moveToNextLine();
                    continue;
                }

                string key = r.readToFirstOf("=");
                string value = r.readLine();
                
                // do something to store section/key/value
            }
        }
        
More samples about StreamReader you can see xml_reader.cpp or json.cpp, both of them are parsing with this StreamReader class.

# Buffer

    Buffer buffer(BufferSize);

    MeasurementDriver* driver = MeasurementDriver::instance();

    for (int startTick = tickcount(); ; msleep(20))
    {
        int numBytes = driver->readMpegStreamData(buffer.end(), buffer.freeSpace());
        if (numBytes > 0) buffer.expand(numBytes);

        while (buffer.remaining() >= PacketSize)
        {
            int code = m_decoder.decode((byte*)buffer.current());

            if (code < 0) // NOT_VALID
            {
                buffer.forward(1);
                continue;
            }
            else if (code == UCD_DETECTED)
            {
                UcdMsg* ucd = m_decoder.ucd();

                logmsg("Ucd detected => %0.2f MHz\n", ucd->frequency / MHZ_F);

                if (isNewUcd(ucd))
                {
                    m_ucds[m_numUcds++] = *ucd;
                    invokeHandler(TestEvent::UcdData, ucd, sizeof(UcdMsg));
                }
            }

            buffer.forward(PacketSize);
        }

        buffer.compact();

        if (tickcount() - startTick > timeout) break;
    }

# XmlProfile

        XmlProfile p;

        p.moveTo("/DocumentRoot");
        p.setChildText("Content", "Settings");
        p.setChildText("Feature", "Setup");        
        p.setChildText("Updated", DateTime::now());

        p.moveTo("Settings");
        p.setChildValue("ChannelTable", m_tableName);
        p.setChildValue("DvbTable",     m_dvbName);
        p.setChildValue("Location",     m_locationName);
        p.setChildValue("Unit",         m_unit());
        p.setChildValue("DeviceName",   m_deviceName);
        p.setChildBool ("RemoteMode",   "enabled",  m_remoteEnabled);
        p.setChildBool ("RemoteMode",   "boot",     m_remoteAutoStart);
        p.setChildInt  ("RemoteMode",   "switching",  m_remoteSwitching);
    
        p.moveTo("SaveOptions");
        p.setChildValue("FileName",     m_saveOptions->name);
        p.setChildValue("JobId",        m_saveOptions->jobId);
        p.setChildValue("NodeId",       m_saveOptions->nodeId);
        p.setChildValue("Account",      m_saveOptions->account);
        p.setChildValue("Location",     m_saveOptions->location);
        p.setChildValue("Comments",     m_saveOptions->comments);
        p.setChildValue("Locator",      m_saveOptions->locator);
        p.setChildValue("SaveType",     m_saveOptions->saveType);
        p.moveToParent();

        p.moveTo("ExportOptions");
        p.setChildValue("Selection",    m_exportOption.selection);
        p.setChildValue("Specified",    m_exportOption.specified);
        p.setChildValue("Existing",     m_exportOption.existing);

        p.save("/path/to/xml/file");

# More
I will add more documents later to explain how to use these library when I get time. Fow now I only have the linux implementations, will add win32 in the future.

