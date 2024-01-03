function getBillDate() {
  const date = new Date()
  return {
    date,
    asString: (otherDate: Date = date) =>
      otherDate.toLocaleDateString("lv-LV", {
        day: "2-digit",
        month: "2-digit",
        year: "numeric",
      }),
    addDays: (count: number) =>
      new Date(date.valueOf() + count * 24 * 60 * 60 * 1000),
  }
}

function ServiceTable({ services }: { services: Service[] }) {
  return (
    <div>
      <div>
        <div className="grid grid-cols-[50px_repeat(5,_minmax(0,_1fr))] bg-slate-300">
          <div></div>
          <div className="p-4">Nosaukums</div>
          <div className="p-4">Daudz.</div>
          <div className="p-4">Mērvienība</div>
          <div className="p-4">Cena</div>
          <div className="p-4">Summa, EUR</div>
        </div>
        <div>
          {services.map((service, id) => (
            <div key={id} className="grid grid-cols-[50px_repeat(5,_minmax(0,_1fr))]  border-b border-slate-400">
              <div className="p-4">{id + 1}</div>
              <div className="p-4">{service.name}</div>
              <div className="p-4">{service.amount}</div>
              <div className="p-4">{service.unit}</div>
              <div className="p-4">{service.price}</div>
              <div className="p-4">{(service.price * service.amount).toFixed(2)}</div>
            </div>
          ))}
          <div className="grid grid-cols-[50px_repeat(5,_minmax(0,_1fr))]">
            <div className="col-span-5 text-right pr-8 p-4">
              Summa apmaksai, EUR
            </div>
            <div className="p-4">
              {services
                .map((service) => service.amount * service.price)
                .reduce((total, current) => total + current)
                .toFixed(2)}
            </div>
          </div>
        </div>
      </div>
    </div>
  )
}

type Service = {
  name: string
  amount: number
  unit: string
  price: number
}

export default function Home() {
  const billNum = 5
  const billDate = getBillDate()
  const contractNo = "ZEN-22-07/01"

  const client = {
    name: 'SIA "Zen IS"',
    address: "Dzirnieku iela 24, Mārupe, Mārupes nov., LV-2167",
    regNo: "42103107097",
    PVNNo: "LV42103107097",
  }

  const provider = {
    name: "Linards Biezbārdis",
    regNo: "250898-11070",
    address: "Rīga, Jaunciema gatve 171-1",
    bank: "SWEDBANK",
    bankAccNo: "LV38HABA0551038903123",
  }

  const services = [
    {
      name: "Drupal izstrādes darbi projektā business.gov.lv",
      amount: 101.18,
      unit: "stundas",
      price: 18,
    },
  ]

  return (
    <main className="flex min-h-screen flex-col items-center gap-8 p-24">
      <div className="flex justify-between w-full">
        <div className="text-4xl border-b-4 border-b-slate-400">
          {provider.name}
        </div>
        <div>
          <div className="grid grid-cols-2 gap-8">
            <span className="font-semibold">Rēķina numurs</span>
            <span>{billNum}</span>
          </div>
          <div className="grid grid-cols-2 gap-8">
            <span className="font-semibold">Rēķina datums</span>
            <span>{billDate.asString()}</span>
          </div>
          <div className="grid grid-cols-2 gap-8">
            <span className="font-semibold">Maksājuma termiņš</span>
            <span>{billDate.asString(billDate.addDays(7))}</span>
          </div>
          <div className="grid grid-cols-2 gap-8">
            <span className="font-semibold">Līguma numurs</span>
            <span>{contractNo}</span>
          </div>
        </div>
      </div>

      <div className="grid grid-cols-2 w-full">
        <div>
          <div className="grid grid-cols-2 gap-8">
            <span className="font-semibold">Klients</span>
            <span>{client.name}</span>
          </div>
          <div className="grid grid-cols-2 gap-8">
            <span className="font-semibold">Reģistrācijas numurs</span>
            <span>{client.regNo}</span>
          </div>
          <div className="grid grid-cols-2 gap-8">
            <span className="font-semibold">PVN numurs</span>
            <span>{client.PVNNo}</span>
          </div>
          <div className="grid grid-cols-2 gap-8">
            <span className="font-semibold">Juridiskā adrese</span>
            <span>{client.address}</span>
          </div>
        </div>
      </div>

      <hr />

      <ServiceTable services={services} />

      <div>
        <div className="uppercase text-xl self-start pb-4 font-semibold">Norēķinu rekvizīti</div>
        <div>
          <div className="grid grid-cols-2 gap-8">
            <span className=" font-semibold">Piegādātājs</span>
            <span>{provider.name}</span>
          </div>
          <div className="grid grid-cols-2 gap-8">
            <span className=" font-semibold">Reģistrācijas numurs</span>
            <span>{provider.regNo}</span>
          </div>
          <div className="grid grid-cols-2 gap-8">
            <span className=" font-semibold">Juridiskā adrese</span>
            <span>{provider.address}</span>
          </div>
          <div className="grid grid-cols-2 gap-8">
            <span className=" font-semibold">Bankas informācija</span>
            <span>{provider.bank}</span>
          </div>
          <div className="grid grid-cols-2 gap-8">
            <span className=" font-semibold">Konta numurs</span>
            <span>{provider.bankAccNo}</span>
          </div>
        </div>
      </div>

      <div>Dokuments ir sagatavots elektroniski un ir derīgs bez paraksta.</div>
    </main>
  )
}
