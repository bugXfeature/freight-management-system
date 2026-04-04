const API = ''; // same origin — frontend and backend now on same port 8080

// ── Live date ──────────────────────────────────────────────────
document.getElementById('live-date').textContent =
  new Date().toLocaleDateString('en-IN', { weekday:'long', year:'numeric', month:'long', day:'numeric' });

// ── Freight type switcher ──────────────────────────────────────
let currentType = 'perishable';

document.querySelectorAll('.type-btn').forEach(btn => {
  btn.addEventListener('click', () => {
    document.querySelectorAll('.type-btn').forEach(b => b.classList.remove('active'));
    document.querySelectorAll('.type-fields').forEach(f => f.classList.remove('active'));

    btn.classList.add('active');
    currentType = btn.dataset.type;
    document.getElementById(`fields-${currentType}`).classList.add('active');
  });
});

// ── Toggle labels ──────────────────────────────────────────────
function bindToggle(checkboxId, labelId, onText, offText) {
  const cb = document.getElementById(checkboxId);
  const lbl = document.getElementById(labelId);
  cb.addEventListener('change', () => {
    lbl.textContent = cb.checked ? onText : offText;
  });
}

bindToggle('tempControl', 'tempLabel', 'Required', 'Not Required');
bindToggle('fragile',     'fragileLabel', 'Yes', 'No');
bindToggle('escort',      'escortLabel',  'Yes', 'No');

// ── Form submit ────────────────────────────────────────────────
document.getElementById('freightForm').addEventListener('submit', async (e) => {
  e.preventDefault();

  const btn = document.querySelector('.submit-btn');
  btn.innerHTML = '<span>CALCULATING…</span>';
  btn.disabled = true;

  const payload = {
    type:     currentType,
    sender:   val('sender'),
    receiver: val('receiver'),
    origin:   val('origin'),
    dest:     val('dest'),
    distance: parseFloat(val('distance')),
    weight:   parseFloat(val('weight')),
  };

  // Append type-specific fields
  if (currentType === 'perishable') {
    payload.tempControl  = document.getElementById('tempControl').checked;
    payload.expiryHours  = parseInt(val('expiryHours') || '48');
  } else if (currentType === 'nonperishable') {
    payload.packaging    = val('packaging');
    payload.fragile      = document.getElementById('fragile').checked;
  } else if (currentType === 'hazardous') {
    payload.hazardClass  = parseInt(val('hazardClass'));
    payload.escort       = document.getElementById('escort').checked;
  }

  try {
    const res = await fetch(`${API}/api/quotation`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(payload)
    });

    const data = await res.json();

    if (!res.ok) {
      alert('Error: ' + (data.error || 'Unknown error'));
      return;
    }

    renderQuotation(data);

  } catch (err) {
    alert('Could not connect to backend. Make sure the server is running on port 8080.\n\n' + err.message);
  } finally {
    btn.innerHTML = '<span>GENERATE QUOTATION</span><span class="btn-arrow">→</span>';
    btn.disabled = false;
  }
});

function val(id) {
  return document.getElementById(id).value.trim();
}

// ── Render quotation card ──────────────────────────────────────
function renderQuotation(d) {
  document.getElementById('q-shipmentId').textContent  = d.shipmentId;
  document.getElementById('q-date').textContent        = d.date;
  document.getElementById('q-sender').textContent      = d.sender;
  document.getElementById('q-origin').textContent      = '📍 ' + d.origin;
  document.getElementById('q-receiver').textContent    = d.receiver;
  document.getElementById('q-dest').textContent        = '📍 ' + d.destination;
  document.getElementById('q-type').textContent        = d.freightType;
  document.getElementById('q-weight').textContent      = d.weightKg + ' kg';
  document.getElementById('q-distance').textContent    = d.distanceKm + ' km';
  document.getElementById('q-base').textContent        = '₹ ' + fmt(d.baseFreight);
  document.getElementById('q-handling').textContent    = '₹ ' + fmt(d.handlingCharge);
  document.getElementById('q-total').textContent       = '₹ ' + fmt(d.totalCost);
  document.getElementById('q-rules').textContent       = d.routeRules;

  const section = document.getElementById('quotationSection');
  section.style.display = 'block';
  section.scrollIntoView({ behavior: 'smooth', block: 'start' });

  // Save for PDF
  window._lastQuotation = d;
}

function fmt(val) {
  return parseFloat(val).toLocaleString('en-IN', { minimumFractionDigits: 2 });
}

// ── PDF Download ───────────────────────────────────────────────
function downloadPDF() {
  const { jsPDF } = window.jspdf;
  const doc = new jsPDF({ unit: 'mm', format: 'a4' });
  const d = window._lastQuotation;
  if (!d) return;

  const W = 210, margin = 20;
  let y = margin;

  // Header bar
  doc.setFillColor(232, 184, 75);
  doc.rect(0, 0, W, 12, 'F');

  doc.setFont('helvetica', 'bold');
  doc.setFontSize(10);
  doc.setTextColor(13, 13, 13);
  doc.text('INDIAN RAILWAY FREIGHT MANAGEMENT SYSTEM', margin, 8);

  y = 24;

  // Title
  doc.setTextColor(30, 30, 30);
  doc.setFontSize(18);
  doc.text('FREIGHT QUOTATION', margin, y);
  y += 7;

  doc.setFontSize(10);
  doc.setFont('helvetica', 'normal');
  doc.setTextColor(100, 100, 100);
  doc.text('Shipment ID: ' + d.shipmentId, margin, y);
  doc.text('Date: ' + d.date, W - margin, y, { align: 'right' });
  y += 10;

  // Divider
  doc.setDrawColor(200, 200, 200);
  doc.line(margin, y, W - margin, y);
  y += 8;

  // Parties
  doc.setFont('helvetica', 'bold');
  doc.setFontSize(9);
  doc.setTextColor(130, 120, 110);
  doc.text('FROM', margin, y);
  doc.text('TO', W - margin - 60, y);
  y += 5;

  doc.setFont('helvetica', 'bold');
  doc.setFontSize(11);
  doc.setTextColor(30, 30, 30);
  doc.text(d.sender, margin, y);
  doc.text(d.receiver, W - margin - 60, y);
  y += 5;

  doc.setFont('helvetica', 'normal');
  doc.setFontSize(9);
  doc.setTextColor(100, 100, 100);
  doc.text(d.origin, margin, y);
  doc.text(d.destination, W - margin - 60, y);
  y += 12;

  // Divider
  doc.line(margin, y, W - margin, y);
  y += 8;

  // Details row
  const details = [
    ['FREIGHT TYPE', d.freightType],
    ['WEIGHT', d.weightKg + ' kg'],
    ['DISTANCE', d.distanceKm + ' km'],
  ];

  details.forEach((item, i) => {
    const x = margin + i * 58;
    doc.setFont('helvetica', 'bold');
    doc.setFontSize(8);
    doc.setTextColor(130, 120, 110);
    doc.text(item[0], x, y);
    doc.setFont('helvetica', 'normal');
    doc.setFontSize(10);
    doc.setTextColor(30, 30, 30);
    doc.text(item[1], x, y + 5);
  });

  y += 16;
  doc.line(margin, y, W - margin, y);
  y += 8;

  // Costs
  const costRows = [
    ['Base Freight', '₹ ' + fmt(d.baseFreight)],
    ['Handling Charge', '₹ ' + fmt(d.handlingCharge)],
  ];

  costRows.forEach(row => {
    doc.setFont('helvetica', 'normal');
    doc.setFontSize(10);
    doc.setTextColor(80, 80, 80);
    doc.text(row[0], margin, y);
    doc.text(row[1], W - margin, y, { align: 'right' });
    y += 7;
  });

  // Total
  doc.setFillColor(232, 184, 75);
  doc.rect(margin, y - 2, W - margin * 2, 10, 'F');
  doc.setFont('helvetica', 'bold');
  doc.setFontSize(12);
  doc.setTextColor(13, 13, 13);
  doc.text('TOTAL', margin + 3, y + 5);
  doc.text('₹ ' + fmt(d.totalCost), W - margin - 3, y + 5, { align: 'right' });
  y += 18;

  // Route rules
  doc.setFont('helvetica', 'bold');
  doc.setFontSize(9);
  doc.setTextColor(130, 120, 110);
  doc.text('ROUTE & HANDLING RULES', margin, y);
  y += 6;

  doc.setFont('helvetica', 'normal');
  doc.setFontSize(9);
  doc.setTextColor(50, 50, 50);
  const lines = doc.splitTextToSize(d.routeRules, W - margin * 2);
  doc.text(lines, margin, y);
  y += lines.length * 5 + 10;

  // Footer
  doc.setDrawColor(200, 200, 200);
  doc.line(margin, y, W - margin, y);
  y += 6;
  doc.setFont('helvetica', 'italic');
  doc.setFontSize(8);
  doc.setTextColor(150, 150, 150);
  doc.text('This quotation is valid for 7 days. All charges in INR. Subject to Railway tariff regulations.', W / 2, y, { align: 'center' });

  // Bottom rail stripe
  doc.setFillColor(232, 184, 75);
  doc.rect(0, 287, W, 10, 'F');

  doc.save(`freight-quotation-${d.shipmentId}.pdf`);
}
